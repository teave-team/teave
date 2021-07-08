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
#include <teave-sv/mgr/mgr.h>
#include <teave/runtime/runtime.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <teave/event/signal_handler.h>
#include <teave/common.h>
#include <teave/setting/prepro.h>
#include <teave/debug/debug.h>

namespace Te = Teave;

namespace Teave_sv {

Act_proc::Act_proc(int pid, int uid, int gid) : pid(pid), uid(uid), gid(gid) {}

So_w::So_w(int so_fd, Te::Epoll_mgr *epoll_mgr)
    : so_fd(so_fd), epoll_mgr(epoll_mgr) {}

So_w::~So_w() {
    epoll_mgr->deregister(so_fd);
    close(so_fd);
}

/*
 * argv[1]: socket path for testing purposes,
 * if not provide it is read from Runtime::get_teave_sv_socket_path()
 */
Mgr::Mgr(int argc, char *argv[]) {
    try {
        err = std::make_unique<Te::Err>(Te::Prepro::get_sv_log_file());
        Te::Err *err_p = err.get(); /* save few .get() calls */
        Te::Debug::setup(err_p);
        Te::setup_sig_globals(
            Te::Proc_type::PROC_TEAVE_SV, err_p,
            std::bind(&Mgr::sig_cb, this, std::placeholders::_1));
        /* epoll is not setup yet, register handler only.
         * The handler is teave/event/singal_handler.cpp::sig_handler */
        Te::setup_signals(Te::Sig_set_act::handler_only);

        user = std::make_unique<Te::User>();
        settings =
            std::make_unique<Te::Settings>(err_p, argc, argv, user.get());
        settings->process();
        socket = std::make_unique<Te::Socket>(
            Te::Runtime::get_teave_sv_socket_path(), false);
        socket->server_setup(true);
        socket->set_opt_passcred();
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        if (chmod(socket->get_path().c_str(), mode) < 0)
            throw Te::Sys_err(errno, "chmod err, path: " + socket->get_path(),
                              TE_ERR_LOC);

        /* register event handler call back */
        epoll_mgr = std::make_unique<Te::Epoll_mgr>();
        epoll_mgr->register_(Te::Epoll_reg(
            socket->get_fd(), std::bind(&Mgr::process_socket_conn_event, this,
                                        std::placeholders::_1)));
        ev_mgr = std::make_unique<Te::Ev_mgr>();
    } catch (std::exception &e) {
        err->log(e.what(), TE_ERR_LOC);
        throw Te::Run_err(string("Startup failed - Mgr ctor caught: ") +
                              e.what(),
                          TE_ERR_LOC);
    }
}

Mgr::~Mgr() { cleanup(); }

/* this is called from signal handler callback as well as dtor */
void Mgr::cleanup() {
    a_p = nullptr;
    so_ws[I_CUR] = nullptr;
    so_ws[I_TEMP] = nullptr;
    close(sig_fd);
    Te::Ev_mgr::set_ev_loop_act(Te::Ev_loop_act::quit);
}

void Mgr::sig_cb(signalfd_siginfo &sig) { cleanup(); }

int Mgr::run() {
    int ret = 0;

    try {
        loop();
        /* we cannot epoll signals from this point onward */
        Te::setup_signals(Te::Sig_set_act::unblock);
        Te::setup_signals(Te::Sig_set_act::handler_only);
    } catch (std::exception &e) {
        err->log(e.what(), TE_ERR_LOC);
        ret = -1;
    }

    return ret;
}

void Mgr::loop() {
    int num_events = 0;
    int epoll_fd = epoll_mgr->get_epoll_fd();
    int epoll_max_num = epoll_mgr->get_max_num_events();
    Te::Epoll_reg *ep_reg;
    int i = 0;

    Te::setup_signals(Te::Sig_set_act::block, sig_fd);
    /* register ep_reg for signals */
    epoll_mgr->register_(
        Te::Epoll_reg(sig_fd, std::bind(&Mgr::process_signal_event, this,
                                        std::placeholders::_1)));

    if (ev_mgr->get_ev_loop_act() == Te::Ev_loop_act::quit)
        return;

    /* main loop */
    for (;;) {
        num_events = epoll_wait(epoll_fd, epoll_mgr->events, epoll_max_num, -1);
        if (num_events < 0)
            throw Te::Sys_err(errno, "epoll_wait err", TE_ERR_LOC);

        for (i = 0; i < num_events; i++) {
            ep_reg = reinterpret_cast<Te::Epoll_reg *>(
                epoll_mgr->events[i].data.ptr);
            if (ep_reg) {
                /* call event handler ep_reg */
                ep_reg->fnc(ep_reg);
                if (ev_mgr->get_ev_loop_act() == Te::Ev_loop_act::quit)
                    return;

            } /* end if(ep_reg) */

        } /* end epoll for num_events loop */
    }     /* end main loop */
}

void Mgr::process_socket_conn_event(Te::Epoll_reg *ep_reg) {
    if (!ep_reg)
        throw Te::Arg_err("ep_reg is null", TE_ERR_LOC);
    try {
        so_ws[I_TEMP] =
            std::make_unique<So_w>(socket->accept(), epoll_mgr.get());
        epoll_mgr->register_(Te::Epoll_reg(
            so_ws[I_TEMP]->so_fd, std::bind(&Mgr::process_accepted_socket_event,
                                            this, std::placeholders::_1)));
        so_ws[I_TEMP]->svs = std::make_unique<Te::Svs>(err.get(), socket.get(),
                                                       so_ws[I_TEMP]->so_fd);
    } catch (std::exception &e) {
        err->log(e.what(), TE_ERR_LOC);
    }
}

void Mgr::process_accepted_socket_event(Te::Epoll_reg *ep_reg) {
    try {
        So_w *sw = nullptr;
        if (so_ws[I_CUR] != nullptr && so_ws[I_CUR]->so_fd == ep_reg->fd)
            sw = so_ws[I_CUR].get();
        else
            sw = so_ws[I_TEMP].get();

        auto [ret_size, ucred_uptr, msg] = sw->svs->recv_ucred_msg(ep_reg->fd);

        if (ret_size == 0) { /* connection is closed */
            if (so_ws[I_CUR] != nullptr && so_ws[I_CUR]->so_fd == ep_reg->fd)
                so_ws[I_CUR] = nullptr;
            else
                so_ws[I_TEMP] = nullptr;
            return;
        }

        ucred *uc = ucred_uptr.get();
        if (!process_proc(sw, msg.get(), uc))
            return;

        if (so_ws[I_CUR] == nullptr || so_ws[I_CUR]->so_fd != ep_reg->fd)
            so_ws[I_CUR] = std::move(so_ws[I_TEMP]);

        switch (msg->code) {
        case Te::Msg::Code::gdev_fd_req:
            process_gdev_fd_req(msg.get(), uc);
            break;
        case Te::Msg::Code::idev_fd_req:
            process_idev_fd_req(msg.get(), uc);
            break;
        case Te::Msg::Code::vt_fd_req:
            process_vt_fd_req(msg.get(), uc);
            break;
        case Te::Msg::Code::fd_req:
            process_fd_req(msg.get(), uc);
            break;
        case Te::Msg::Code::quit_req:
            process_quit_req(msg.get(), uc);
            break;
        default:
            break;
        };
    } catch (std::exception &e) {
        err->log(e.what(), TE_ERR_LOC);
    }
}

bool Mgr::process_proc(So_w *sw, Te::Msg *msg, ucred *uc) {
    if (!sw)
        throw Te::Arg_err("sw is null", TE_ERR_LOC);
    if (!msg)
        throw Te::Arg_err("msg is null", TE_ERR_LOC);
    if (!uc)
        throw Te::Arg_err("uc is null", TE_ERR_LOC);

    /* if active process   */
    if (a_p == nullptr) {
        a_p = std::make_unique<Act_proc>(uc->pid, uc->uid, uc->gid);
        return true;
    }

    /*
    in prod mode: make sure only one process (active process) can
    request action
    in dev mode: replace the active process with the current
    requesting process
    */
    if (settings->get_mode() == Te::Mode::prod) {
        if (a_p->pid == uc->pid) {
            return true;
        } else {
            res_err(sw, Te::Msg::Err_code::another_act_proc);
            return false;
        }
    } else {
        a_p = std::make_unique<Act_proc>(uc->pid, uc->uid, uc->gid);
        return true;
    }
    return false;
}

void Mgr::process_gdev_fd_req(Te::Msg *msg, ucred *uc) {
    string path(msg->buffer);
    try {
        /* if dev exists, destroy it (this will drop drm master and close fd,
         * see Teave_sv::Dev::~Dev() ) then create a new one */
        a_p->gdevs[path] = std::make_unique<Gdev>(err.get(), path, msg->flags);
        Te::Msg res_msg(Te::Msg::Code::fd_res, path, 0);
        so_ws[I_CUR]->svs->send_fd(a_p->gdevs[path]->fd, res_msg);
    } catch (std::exception &e) {
        err->log(e.what() + string(" ") + Te::uc_to_str(uc), TE_ERR_LOC);
        Te::Msg res_msg(Te::Msg::Code::err_res, Te::Msg::Err_code::failed, path,
                        0);
        so_ws[I_CUR]->svs->send_msg(res_msg);
    }
}

void Mgr::process_idev_fd_req(Te::Msg *msg, ucred *uc) {
    string path(msg->buffer);
    try {
        a_p->idevs[path] =
            std::make_unique<Dev>(err.get(), path, Dev::Type::idev, msg->flags);
        Te::Msg res_msg(Te::Msg::Code::fd_res, path, 0);
        so_ws[I_CUR]->svs->send_fd(a_p->idevs[path]->fd, res_msg);
    } catch (std::exception &e) {
        err->log(e.what() + string(" ") + Te::uc_to_str(uc), TE_ERR_LOC);
        Te::Msg res_msg(Te::Msg::Code::err_res, Te::Msg::Err_code::failed, path,
                        0);
        so_ws[I_CUR]->svs->send_msg(res_msg);
    }
}

void Mgr::process_vt_fd_req(Te::Msg *msg, ucred *uc) {
    string path(msg->buffer);
    try {
        a_p->vt =
            std::make_unique<Dev>(err.get(), path, Dev::Type::dev, msg->flags);
        Te::Msg res_msg(Te::Msg::Code::fd_res, path, 0);
        so_ws[I_CUR]->svs->send_fd(a_p->vt->fd, res_msg);
    } catch (std::exception &e) {
        err->log(e.what() + string(" ") + Te::uc_to_str(uc), TE_ERR_LOC);
        Te::Msg res_msg(Te::Msg::Code::err_res, Te::Msg::Err_code::failed, path,
                        0);
        so_ws[I_CUR]->svs->send_msg(res_msg);
    }
}

void Mgr::process_fd_req(Te::Msg *msg, ucred *uc) {
    string path(msg->buffer);
    try {
        a_p->devs[path] =
            std::make_unique<Dev>(err.get(), path, Dev::Type::dev, msg->flags);
        Te::Msg res_msg(Te::Msg::Code::fd_res, path, 0);
        so_ws[I_CUR]->svs->send_fd(a_p->devs[path]->fd, res_msg);
    } catch (std::exception &e) {
        err->log(e.what() + string(" ") + Te::uc_to_str(uc), TE_ERR_LOC);
        Te::Msg res_msg(Te::Msg::Code::err_res, Te::Msg::Err_code::failed, path,
                        0);
        so_ws[I_CUR]->svs->send_msg(res_msg);
    }
}

void Mgr::process_quit_req(Te::Msg *msg, ucred *uc) {
    /* cred checks are already done in Mgr::check_creds */
    a_p = nullptr;
    so_ws[I_CUR] = nullptr;
}

void Mgr::process_signal_event(Te::Epoll_reg *ep_reg) {
    signalfd_siginfo sig;
    if (read(sig_fd, &sig, sizeof sig) != sizeof sig)
        throw Te::Sys_err(errno, "read err", TE_ERR_LOC);

    if (sig.ssi_signo == SIGINT) {
        Te::Ev_mgr::set_ev_loop_act(Te::Ev_loop_act::quit);
        Te::Ev_mgr::set_ev_type(Te::Ev_type::sig);
        return;
    }

    Te::process_signal(sig);
}

void Mgr::res_err(So_w *sw, Te::Msg::Err_code e_code) {
    Te::Msg res_msg(Te::Msg::Code::err_res);
    res_msg.e_code = e_code;
    sw->svs->send_msg(res_msg);
}

} /* namespace Teave_sv */