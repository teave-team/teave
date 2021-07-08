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
#ifndef TEAVE_SV_H
#define TEAVE_SV_H

#include <teave/err/err.h>
#include <teave/socket/socket.h>
#include <teave/sv/msg.h>

namespace Teave {
using std::string;

class Sv {
public:
    Sv(Err *const err, Socket *socket, int fd);
    void send_msg(Msg &msg, int flags = 0);
    std::tuple<ssize_t, Msg::uptr> recv_msg();
    std::tuple<ssize_t, std::unique_ptr<ucred>, Msg::uptr>
    recv_ucred_msg(int from_fd);
    void req_quit();

protected:
    Err *const err = nullptr;
    Socket *socket = nullptr;
    /* Fd to be used for socket calls.
     * When Teave-sv is using this class, the fd would be the accepted fd after
     * app has requested a connection from Teave-sv When App is using this
     * class, the fd would be the socket's fd
     */
    int fd = -1;
};

} // namespace Teave

#endif /* TEAVE_SV_H */
