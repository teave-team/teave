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
#include <teave/sv/sva.h>
#include <teave/sv/util.h>

namespace Teave {
Sva::Sva(Err *const err, Socket *socket, int fd) : Sv(err, socket, fd) {}
std::tuple<bool, Msg::uptr, int> Sva::req_gdev_fd(string &path,
                                                  int open_flags) {
    Msg msg(Msg::Code::gdev_fd_req, path, open_flags);
    return req_fd(msg);
}

std::tuple<bool, Msg::uptr, int> Sva::req_idev_fd(string &path,
                                                  int open_flags) {
    Msg msg(Msg::Code::idev_fd_req, path, open_flags);
    return req_fd(msg);
}

std::tuple<bool, Msg::uptr, int> Sva::req_vt_fd(string &path, int open_flags) {
    Msg msg(Msg::Code::vt_fd_req, path, open_flags);
    return req_fd(msg);
}

std::tuple<bool, Msg::uptr, int> Sva::req_fd(string &path, int open_flags) {
    Msg msg(Msg::Code::fd_req, path, open_flags);
    return req_fd(msg);
}

std::tuple<bool, Msg::uptr, int> Sva::req_fd(Msg &msg) {
    bool ret = true;
    send_msg(msg, 0);
    ssize_t ret_size = 0;
    Msg::uptr ret_msg;
    int ret_fd = -1;
    try {
        std::tie(ret_size, ret_msg, ret_fd) = recv_fd();
    } catch (std::exception &e) {
        throw Run_err(
            "recv_fd err. original send_msg msg: " + msg_code_to_str(&msg) +
                ", err_code: " + msg_err_code_to_str(&msg) +
                ", caught:" + e.what(),
            TE_ERR_LOC);
    }

    if (ret_size == 0)
        throw Run_err("ret_size is 0, expecting fd_res, after req msg: " +
                          msg_path_err_code_to_str(&msg),
                      TE_ERR_LOC);

    if (string(msg.buffer) != string(ret_msg->buffer))
        throw Run_err(
            "expecting ret_msg path(ie. buffer) == " + string(msg.buffer) +
                " got: " + string(ret_msg->buffer) +
                " req msg: " + msg_path_err_code_to_str(&msg) +
                ", ret msg: " + msg_path_err_code_to_str(ret_msg.get()),
            TE_ERR_LOC);

    if (ret_msg->code == Msg::Code::err_res) {
        ret = false;
        err->log(
            "recv_er_fd_msg err, req msg: " + msg_path_err_code_to_str(&msg) +
                ", ret msg: " + msg_path_err_code_to_str(ret_msg.get()),
            TE_ERR_LOC);
    } else if (ret_msg->code != Msg::Code::fd_res) {
        ret = false;
        err->log("ret_msg->code != Msg::Code::fd_res , req msg: " +
                     msg_path_err_code_to_str(&msg) +
                     ", ret msg: " + msg_path_err_code_to_str(ret_msg.get()),
                 TE_ERR_LOC);
    }

    return {ret, std::move(ret_msg), ret_fd};
}

std::tuple<ssize_t, Msg::uptr, int> Sva::recv_fd() {
    Msg::uptr msg = std::make_unique<Msg>();
    ssize_t ret_size = 0;
    int ret_fd = -1;
    std::tie(ret_size, ret_fd) = socket->recv_fd(fd, msg.get(), sizeof(Msg));
    return {ret_size, std::move(msg), ret_fd};
}

} /* namespace Teave */