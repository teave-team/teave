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
#include <teave/sv/msg.h>

namespace Teave {
Msg::Msg() {}

Msg::Msg(Code code) : code(code) {}

Msg::Msg(Code code, string &str, int flags)
    : Msg(code, Err_code::none, str, flags) {}

Msg::Msg(Code code, Err_code e_code, string &str, int flags)
    : code(code), e_code(e_code), flags(flags) {
    std::memset(buffer, 0, sizeof(buffer));
    size_t str_size = str.size() + 1;

    if (str_size > MSG_BUF_SIZE)
        throw Run_err("str is too long, FD_PATH_BUF_SIZE: " +
                          std::to_string(MSG_BUF_SIZE) +
                          ", str_size: " + std::to_string(str_size),
                      TE_ERR_LOC);
    std::memcpy(buffer, str.c_str(), str_size);
}

} // namespace Teave