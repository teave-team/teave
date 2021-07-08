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
#ifndef TEAVE_SV_MSG_H
#define TEAVE_SV_MSG_H

#include <stdlib.h>
#include <string>
#include <teave/err/err.h>
#include <cstring>
#include <xf86drm.h>

#define MSG_NUM_FD 1 // number of fds passed via socket

#define MSG_BUF_SIZE 100

namespace Teave {
using std::string;
/*
 * Socket message
 */
struct Msg {
    using uptr = std::unique_ptr<Msg>;
    /* message code (App: Teave, Er: Teave, Es: Teave_starter) */
    enum class Code {
        gdev_fd_req, /* graphics device fd request from App to Teave-sv */
        idev_fd_req, /* input device fd request from App to Teave-sv */
        vt_fd_req,   /* open virtual terminal fd */
        fd_req,      /* device fd request from App to Teave-sv */
        fd_res,      /* fd response from Teave-sv to App */
        quit_req,    /* from App to Teave-sv */
        err_res,     /* can be sent from both sides */
        none
    };
    enum class Err_code {
        another_act_proc, /* another active process exists */
        gdev_not_found,   /* gdev is not open (need to gdev_fd_req first) */
        vt_not_found,     /* process has not opened a vt yet */
        already_done_no_changes, /* no change was made, the action had already
                                    been requested */
        failed,                  /* system calls failed */
        req_was_none,            /* request code was none */
        none
    };
    Msg();
    Msg(Code code);
    Msg(Code code, string &str, int flags = -1);
    Msg(Code code, Err_code e_code, string &str, int flags = -1);
    Code code = Code::none;
    Err_code e_code = Err_code::none;
    int flags = -1;
    char buffer[MSG_BUF_SIZE];
    size_t buf_c_size = 0; // size of content in buffer

    /* for Gdev */
    int max_width = 0;
    int min_width = 0;
    int max_height = 0;
    int min_height = 0;
    drm_magic_t magic = 0;
};

} // namespace Teave

#endif /* TEAVE_SV_MSG_H */