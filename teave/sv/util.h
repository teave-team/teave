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
#ifndef TEAVE_SV_UTIL_H
#define TEAVE_SV_UTIL_H

#include <teave/socket/socket.h>
#include <teave/sv/msg.h>
#include <string>

namespace Teave {
using std::string;
string uc_to_str(ucred *uc);
string msg_path_err_code_to_str(Msg *msg);
string msg_code_and_err_code(Msg *msg);
string msg_code_to_str(Msg *msg);
string msg_err_code_to_str(Msg *msg);
} // namespace Teave
#endif /* TEAVE_SV_UTIL_H */
