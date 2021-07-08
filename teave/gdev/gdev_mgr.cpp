/*
 * Copyright 2021 Pedram Tiv
 *
 * Licensed under the Teave Public License (the "License");
 * You may obtain a copy of the License at https://www.teave.io/teave-public-license.html
 *
 * This software is distributed under the License on an "AS IS" BASIS, WITHOUT WARRANTIES 
 * OF ANY KIND, either express or implied. See the License for the specific language governing 
 * terms and conditions for use, reproduction, and distribution.
 */
#include <teave/gdev/gdev_mgr.h>
#include <sys/mman.h>
#include <locale>
#include <teave/gdev/gdev_helpers.h>
#include <algorithm>
#include <teave/debug/debug.h>

namespace Teave {
using std::string;
using vec_str = std::vector<string>;

Gdev_mgr::Gdev_mgr(Err *const err, Settings *const settings,
                   Epoll_mgr *const epoll_mgr, Sva *const sva)
    : err(err), settings(settings), epoll_mgr(epoll_mgr), sva(sva) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    if (!settings)
        throw Arg_err("settings is null", TE_ERR_LOC);
    if (!epoll_mgr)
        throw Arg_err("epoll_mgr is null", TE_ERR_LOC);
    if (!sva)
        throw Arg_err("sva is null", TE_ERR_LOC);
    load_devs();

    drm_ec.version = 4;
    drm_ec.vblank_handler = te_vblank_handler;
}

Gdev_mgr::~Gdev_mgr() {
    for (auto &dev : devs) {
        epoll_mgr->deregister(dev->dev_f->fd);
    }

    devs.clear();
}

void Gdev_mgr::load_devs() {
    Udev_enum ud_en(udev_w.ud);
    udev_list_entry *first_item = ud_en.get_list("drm", "card[0-9]*");
    if (!first_item)
        throw Run_err("udev_w.get_list returned null", TE_ERR_LOC);

    std::vector<string> tmp_node_paths;

    int i = 0;
    /* gcon id: id of connector, does not reset for each gdev.
    If there are 2 gdevs and each have 3 connected screens,
    then the con_ids are from 0 to 5 */
    int con_id = 0; 
    for (udev_list_entry *item = first_item; item != NULL;
         item = udev_list_entry_get_next(item)) {
        const char *sys_path = udev_list_entry_get_name(item);
        if (!sys_path) {
            err->log("udev_list_entry_get_name returned null", TE_ERR_LOC);
            continue;
        }

        Udev_dev ud_dev(udev_device_new_from_syspath(udev_w.ud, sys_path));
        if (!ud_dev.dev) {
            err->log(string("udev_device_new_from_syspath returned null for "
                            "sys_path: ") +
                         sys_path,
                     TE_ERR_LOC);
            continue;
        }

        const char *node_path = udev_device_get_devnode(ud_dev.dev);
        if (node_path) {
            /*
             * list of devices include graphic cards and connectors,
             * we are only looking for cards at this point.
             * ex:
             * /sys/devices/pci0000:00/0000:00:02.0/drm/card0
             * /sys/devices/pci0000:00/0000:00:02.0/drm/card0/card0-DP-1
             * only need: /sys/devices/pci0000:00/0000:00:02.0/drm/card0
             *
             * ignore if node_path already exists
             */
            if (std::find(tmp_node_paths.begin(), tmp_node_paths.end(),
                          node_path) != tmp_node_paths.end())
                continue;

            Dev_f::uptr dev_f = std::make_unique<Dev_f>(node_path);

            /* note for calls to sva, sva does the err logging. We just
            skip this gdev in case of an error */
            bool ret = false;
            Msg::uptr ret_fd_msg = nullptr;
            std::tie(ret, ret_fd_msg, dev_f->fd) =
                sva->req_gdev_fd(dev_f->path);
            if (!ret)
                continue;

            Gdev::uptr gdev =
                std::make_unique<Gdev>(err, std::move(dev_f), settings);
            gdev->id = i;
            i++;
            setup_dev(gdev.get(), con_id);
            devs.emplace_back(std::move(gdev));
            tmp_node_paths.push_back(node_path);
        }
    } /* end for */

    load_num_cons();
}

/* pass con_id by ref, since we are updating the global con_id */
void Gdev_mgr::setup_dev(Gdev *dev, int& con_id) {
    Drm_mode_res dr(dev->dev_f->fd, dev->dev_f->path);
    dev->min_width = dr.res->min_width;
    dev->max_width = dr.res->max_width;
    dev->min_height = dr.res->min_height;
    dev->max_height = dr.res->max_height;

    load_dev_cons(err, devs, dev, dr.res);
    for (auto &con : dev->cons) {
        con->id = con_id;
        con->name = "screen" + std::to_string(con->id);
        con_id++;
    }

    if (dev->cons.size() > 1) {
        /* set surface width and height to the largest width and height in all
         * cons */
        for (auto &con : dev->cons) {
            if (con->width > dev->eg_dev.sur_width)
                dev->eg_dev.sur_width = con->width;
            if (con->height > dev->eg_dev.sur_height)
                dev->eg_dev.sur_height = con->height;
        }

        dev->multi_con = true;
    } else {
        dev->eg_dev.sur_width = dev->cons.front()->width;
        dev->eg_dev.sur_height = dev->cons.front()->height;
        dev->multi_con = false;
    }

    /* epoll for graphics card events (page flips) */
    epoll_mgr->register_(Epoll_reg(
        dev->dev_f->fd,
        std::bind(&Gdev_mgr::process_event, this, std::placeholders::_1), dev));

    int ret = drmSetClientCap(dev->dev_f->fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if (ret)
        throw Run_err("drmSetAppCap for DRM_CLIENT_CAP_ATOMIC failed",
                      TE_ERR_LOC);

    dev->ready = true;
}

void Gdev_mgr::process_event(Epoll_reg *ep_reg) {
    int ret = 0;
    do {
        /* calls page flip handler - see drm.cpp te_page_flip_handler. drm_ec is
         * set in ctor */
        ret = drmHandleEvent(ep_reg->fd, &drm_ec);
    } while (ret > 0);

    if (ret < 0) {
        if (!ep_reg->data)
            throw Run_err("drmHandleEvent failed and ep_reg->data is null",
                          TE_ERR_LOC);
        throw Run_err("drmHandleEvent failed for " +
                          gdev_info(static_cast<Gdev *>(ep_reg->data)),
                      TE_ERR_LOC);
    }
}

void Gdev_mgr::schedule_vblank(Gcon *con) {
    drmVBlank vbl;
    vbl.request.type = static_cast<drmVBlankSeqType>(
        DRM_VBLANK_RELATIVE | DRM_VBLANK_EVENT | DRM_VBLANK_NEXTONMISS);
    vbl.request.sequence = 1;

    if (con->id == 1)
        vbl.request.type = static_cast<drmVBlankSeqType>(vbl.request.type |
                                                         DRM_VBLANK_SECONDARY);

    vbl.request.signal = reinterpret_cast<unsigned long>(con);
    int ret = drmWaitVBlank(con->dev->dev_f->fd, &vbl);
    if (ret != 0)
        throw Sys_err(errno,
                      "drmWaitVBlank failed, dev: " + gdev_info(con->dev) +
                          " con: " + gdev_con_info(con),
                      TE_ERR_LOC);
}

void Gdev_mgr::load_num_cons() {
    num_cons = 0;
    for (auto &dev : devs) {
        num_cons += dev->cons.size();
    }

    if (num_cons > MAX_GDEV_CONS) {
        num_cons = MAX_GDEV_CONS;

        if (err->get_logger()->get_log_lvl() == Log_lvl::info) {
            err->log(string("num of cons is greater than MAX_GDEV_CONS,") +
                         " num_cons: " + std::to_string(num_cons) +
                         " MAX_GDEV_CONS: " + std::to_string(MAX_GDEV_CONS) +
                         " only " + std::to_string(MAX_GDEV_CONS) +
                         " will be used",
                     TE_CUR_LOC);
        }
    }
}

int Gdev_mgr::get_num_cons() { return num_cons; }
/* for debuging purposes */
void Gdev_mgr::list_info() {
    for (Gdev::uptr &d : devs) {

        Debug::log("dev info: ", TE_ERR_LOC);
        Debug::log("d->dev_f->fd: " + std::to_string(d->dev_f->fd), "");
        Debug::log("d->dev_f->path: " + d->dev_f->path, "");

        int mode_count = 0;
        for (Gcon::uptr &con : d->cons) {
            Debug::log("con->width" + std::to_string(con->width), "");
            Debug::log("con->height" + std::to_string(con->height), "");

            mode_count = 0;
            Drm_con d_con(d->dev_f->fd, con->gdev_con_id);
            mode_count = d_con.con->count_modes;
            for (int i = 0; i < mode_count; i++) {
                Debug::log("d_con.con->modes[i].name: " +
                               string(d_con.con->modes[i].name),
                           "");

                Debug::log("d_con.con->modes[i].hdisplay: " +
                               std::to_string(d_con.con->modes[i].hdisplay),
                           "");
                Debug::log("d_con.con->modes[i].vdisplay: " +
                               std::to_string(d_con.con->modes[i].vdisplay),
                           "");

                Debug::log("d_con.con->modes[i].htotal: " +
                               std::to_string(d_con.con->modes[i].htotal),
                           "");
                Debug::log("d_con.con->modes[i].vtotal: " +
                               std::to_string(d_con.con->modes[i].vtotal),
                           "");
            }
        }

        Debug::log("dev info end. ", "");
    }
}

} // namespace Teave
