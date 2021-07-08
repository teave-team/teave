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
#include <teave/gdev/gdev_helpers.h>
#include <teave/gdev/drm.h>
#include <cstring>

using std::string;

namespace Teave {

string gdev_info(Gdev *dev) {
    if (dev)
        return string(" dev->dev_f->path: ") + dev->dev_f->path +
               string(" fd:") + std::to_string(dev->dev_f->fd);
    return {};
}

string gdev_con_info(Gcon *con) {
    using string = std::string;
    if (con)
        return string("con->with: " + std::to_string(con->width) +
                      string(" con->height: ") + std::to_string(con->height));
    return {};
}

void load_dev_cons(Err *err, std::vector<Gdev::uptr> &devs, Gdev *dev,
                   drmModeRes *res) {
    if (!dev)
        throw Arg_err("dev is null", TE_ERR_LOC);
    if (!res)
        throw Arg_err("res is null", TE_ERR_LOC);

    int crtc_id = -1;

    bool connected_out = false; // flag for existence of a connected output

    for (int i = 0; i < res->count_connectors; i++) {

        Drm_con drm_con(dev->dev_f->fd, res->connectors[i]);
        if (!drm_con.con)
            continue;

        if (drm_con.con->connection == DRM_MODE_CONNECTED) {

            connected_out = true;
            crtc_id = get_crtc(devs, dev, res, drm_con.con);
            if (crtc_id < 0) {
                err->log("get_crtc failed, dev->dev_f->path: " +
                             dev->dev_f->path,
                         TE_ERR_LOC);

            } else {
                Gcon::uptr con = std::make_unique<Gcon>(err, dev);
                con->gdev_con_id = drm_con.con->connector_id;
                con->crtc_id = crtc_id;

                std::memcpy(&con->mode, &drm_con.con->modes[0],
                            sizeof(con->mode));
                con->width = drm_con.con->modes[0].hdisplay;
                con->height = drm_con.con->modes[0].vdisplay;
                con->saved_crtc = drmModeGetCrtc(dev->dev_f->fd, con->crtc_id);
                dev->cons.push_back(std::move(con));
            }
        }

    } /* end for */

    int size = dev->cons.size();

    if (size == 0 && connected_out == true)
        err->log("Connected output(s) found but was not able to load a "
                 "connector for dev, " +
                     string("dev->dev_f->path: ") + dev->dev_f->path,
                 TE_ERR_LOC);
}

/*
 * returns crtc_id on success (positive int32_t)
 * returns -1 on failure
 */
int32_t get_crtc(std::vector<Gdev::uptr> &devs, Gdev *dev, drmModeRes *res,
                 drmModeConnector *con) {
    /* first try the currently connected encoder */
    if (con->encoder_id) {
        Drm_enc enc_cur = {
            .enc = drmModeGetEncoder(dev->dev_f->fd, con->encoder_id)};

        if (enc_cur.enc) {
            if (enc_cur.enc->crtc_id &&
                !is_crtc_in_use(devs, enc_cur.enc->crtc_id))
                return enc_cur.enc->crtc_id;
        }
    }

    /* try other encoders available to the connector */
    for (int i = 0; i < con->count_encoders; i++) {

        Drm_enc enc_t = {
            .enc = drmModeGetEncoder(dev->dev_f->fd, con->encoders[i])};
        if (!enc_t.enc)
            continue;

        /* go through available crtcs in resources (global resources) */
        for (int j = 0; j < res->count_crtcs; j++) {

            /* is this crtc a possibility for this encoder */
            if (!(enc_t.enc->possible_crtcs & (1 << j)))
                continue;

            if (!is_crtc_in_use(devs, res->crtcs[j]))
                return res->crtcs[j];
        }
    }

    return -1;
}

bool is_crtc_in_use(std::vector<Gdev::uptr> &devs, uint32_t crtc_id) {
    for (auto &dev : devs) {
        for (auto &con : dev->cons) {
            if (con->crtc_id == crtc_id)
                return true;
        }
    }

    return false;
}

void restore_saved_crtc(Gdev *dev, Gcon *con) {
    int ret = drmModeSetCrtc(dev->dev_f->fd, con->saved_crtc->crtc_id,
                             con->saved_crtc->buffer_id, con->saved_crtc->x,
                             con->saved_crtc->y, &con->gdev_con_id, 1,
                             &con->saved_crtc->mode);
    if (ret < 0)
        dev->err->log(errno,
                      "drmModeSetCrtc err, dev path: " + con->dev->dev_f->path,
                      TE_ERR_LOC);
}

} // namespace Teave
