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
#include <teave/sv/sv.h>
#include <teave/runtime/runtime.h>
#include <teave/sv/util.h>

namespace Teave {
Sv::Sv(Err *const err, Socket *socket, int fd)
    : err(err), socket(socket), fd(fd) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    if (!socket)
        throw Arg_err("socket is null", TE_ERR_LOC);
    if (fd < 0)
        throw Arg_err("invalid fd", TE_ERR_LOC);
}

void Sv::send_msg(Msg &msg, int flags) {
    try {
        socket->send_data(fd, (void *)&msg, sizeof(Msg), flags);
    } catch (std::exception &e) {
        throw Run_err("Msg::Code: " + msg_code_to_str(&msg) + ", err_code: " +
                          msg_err_code_to_str(&msg) + ", caught:" + e.what(),
                      TE_ERR_LOC);
    }
}

/*
 * using socket which throws exception
 */
std::tuple<ssize_t, Msg::uptr> Sv::recv_msg() {
    Msg::uptr msg = std::make_unique<Msg>();
    ssize_t ret_size = socket->recv_data(fd, msg.get(), sizeof(Msg));
    return {ret_size, std::move(msg)};
}

std::tuple<ssize_t, std::unique_ptr<ucred>, Msg::uptr>
Sv::recv_ucred_msg(int from_fd) {
    Msg::uptr msg = std::make_unique<Msg>();
    ssize_t ret_size = 0;
    std::unique_ptr<ucred> ret_ucred = nullptr;
    std::tie(ret_size, ret_ucred) =
        socket->recv_ucred_data(from_fd, msg.get(), sizeof(Msg));
    return {ret_size, std::move(ret_ucred), std::move(msg)};
}

void Sv::req_quit() {
    Msg msg(Msg::Code::quit_req);
    send_msg(msg, 0);
}

} /* namespace Teave */