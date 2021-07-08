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
#include <teave/sv/util.h>

namespace Teave {
string uc_to_str(ucred *uc) {
    if (!uc)
        throw Arg_err("uc is null", TE_ERR_LOC);
    return "pid: " + std::to_string(uc->pid) +
           " uid: " + std::to_string(uc->uid) +
           " gid: " + std::to_string(uc->gid);
}

string msg_path_err_code_to_str(Msg *msg) {
    if (!msg)
        throw Arg_err("msg is null", TE_ERR_LOC);

    return "path: " + string(msg->buffer) + " " + msg_code_and_err_code(msg);
}

string msg_code_and_err_code(Msg *msg) {
    return "msg code: " + msg_code_to_str(msg) +
           ", msg err code: " + msg_err_code_to_str(msg);
}

string msg_code_to_str(Msg *msg) {
    if (!msg)
        throw Arg_err("msg is null", TE_ERR_LOC);
    switch (msg->code) {
    case Msg::Code::gdev_fd_req:
        return "Msg::Code::gdev_fd_req";
    case Msg::Code::idev_fd_req:
        return "Msg::Code::idev_fd_req";
    case Msg::Code::vt_fd_req:
        return "Msg::Code::vt_fd_req";
    case Msg::Code::fd_req:
        return "Msg::Code::fd_req";
    case Msg::Code::fd_res:
        return "Msg::Code::fd_res";
        return "Msg::Code::vt_in_res";
    case Msg::Code::quit_req:
        return "Msg::Code::quit_req";
    case Msg::Code::err_res:
        return "Msg::Code::err_res";
    case Msg::Code::none:
        return "Msg::Code::none";
    }

    return "";
}

string msg_err_code_to_str(Msg *msg) {
    if (!msg)
        throw Arg_err("msg is null", TE_ERR_LOC);
    switch (msg->e_code) {
    case Msg::Err_code::another_act_proc:
        return "Msg::Err_code::another_act_proc";
    case Msg::Err_code::gdev_not_found:
        return "Msg::Err_code::gdev_not_found";
    case Msg::Err_code::vt_not_found:
        return "Msg::Err_code::vt_not_found";
    case Msg::Err_code::already_done_no_changes:
        return "Msg::Err_code::already_done_no_changes";
    case Msg::Err_code::failed:
        return "Msg::Err_code::failed";
    case Msg::Err_code::req_was_none:
        return "Msg::Err_code::req_was_none";
    case Msg::Err_code::none:
        return "Msg::Err_code::none";
    }
    return "";
}
} // namespace Teave