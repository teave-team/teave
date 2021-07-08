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
#include <teave/sv/svs.h>
#include <teave/sv/util.h>

namespace Teave {
Svs::Svs(Err *const err, Socket *socket, int accept_fd)
    : Sv(err, socket, accept_fd) {}

void Svs::send_fd(int msg_fd, Msg &msg, int flags) {
    try {
        socket->send_fd(fd, msg_fd, (void *)&msg, sizeof(Msg), flags);
    } catch (std::exception &e) {
        throw Run_err("Msg::Code: " + msg_code_to_str(&msg) + ", err_code: " +
                          msg_err_code_to_str(&msg) + ", caught:" + e.what(),
                      TE_ERR_LOC);
    }
}

} /* namespace Teave */