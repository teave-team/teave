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
#ifndef TEAVE_SV_MGR_H
#define TEAVE_SV_MGR_H

#include <teave/epoll/epoll_mgr.h>
#include <teave/event/ev_mgr.h>
#include <teave/sv/svs.h>
#include <teave/sv/util.h>
#include <teave-sv/dev/dev.h>
#include <teave/user/user.h>
#include <teave/setting/settings.h>
#include <teave/event/signal_handler.h>

namespace Te = Teave;

namespace Teave_sv {

/* socket connection wrapper */
struct So_w {
    using uptr = std::unique_ptr<So_w>;
    So_w(const So_w &a) = delete;
    So_w &operator=(const So_w &a) = delete;

    So_w(int so_fd, Te::Epoll_mgr *epoll_mgr);
    ~So_w();
    int so_fd = -1; /* socket accept fd */
    Te::Epoll_mgr *epoll_mgr = nullptr;
    Te::Svs::uptr svs = nullptr;
};

enum So_w_index {
    I_CUR = 0, /* index of current fd */
    I_TEMP = 1 /* index of temp fd */
};

/* active process */
struct Act_proc {
    using uptr = std::unique_ptr<Act_proc>;
    Act_proc(const Act_proc &a) = delete;
    Act_proc &operator=(const Act_proc &a) = delete;

    Act_proc(int pid, int uid, int gid);
    int pid = -1;
    int uid = -1;
    int gid = -1;
    Dev::uptr vt;                                 /* virtual terminal */
    std::unordered_map<string, Gdev::uptr> gdevs; /* umap<path, graphics dev> */
    std::unordered_map<string, Dev::uptr> idevs;  /* umap<path, input dev> */
    std::unordered_map<string, Dev::uptr> devs;   /* umap<path, generic dev> */
};

class Mgr {
public:
    Mgr(int argc, char *argv[]);
    ~Mgr();
    void cleanup();
    void sig_cb(signalfd_siginfo &sig);
    int run();
    void loop();
    void process_socket_conn_event(Te::Epoll_reg *ep_reg);
    void process_accepted_socket_event(Te::Epoll_reg *ep_reg);
    void process_signal_event(Te::Epoll_reg *ep_reg);

private:
    bool process_proc(So_w *sw, Te::Msg *msg, ucred *uc);
    /* requests from Teave */
    void process_gdev_fd_req(Te::Msg *msg, ucred *uc);
    void process_idev_fd_req(Te::Msg *msg, ucred *uc);
    void process_vt_fd_req(Te::Msg *msg, ucred *uc);
    void process_fd_req(Te::Msg *msg, ucred *uc);
    void process_quit_req(Te::Msg *msg, ucred *uc);
    /* end requests from Teave */
    void res_err(So_w *sw, Te::Msg::Err_code e_code);

    Te::User::uptr user = nullptr;
    Te::Err::uptr err = nullptr;
    Te::Socket::uptr socket = nullptr;
    Te::Epoll_mgr::uptr epoll_mgr = nullptr;
    Te::Ev_mgr::uptr ev_mgr = nullptr;
    Te::Settings::uptr settings = nullptr;
    int sig_fd; /* for signal handling */

    std::array<So_w::uptr, 2> so_ws;

    Act_proc::uptr a_p;
};

} // namespace Teave_sv

#endif /* TEAVE_SV_MGR_H */
