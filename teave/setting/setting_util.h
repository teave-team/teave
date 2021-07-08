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
#ifndef TEAVE_SETTING_UTIL_H
#define TEAVE_SETTING_UTIL_H

#include <string>
#include <unordered_map>

namespace Teave {
namespace Setting_util {
using std::string;
string get_parse_err_str(string &&msg, int line_num, string &line);
string get_err_msg(string &&key, string &&value);
string get_err_msg(string &key, string &value);
string get_err_msg_settings_depend(string &&key1, string &&value1,
                                   string &&key2, string &&value2);
string get_err_msg_settings_depend(string &key1, string &value1, string &key2,
                                   string &value2);
void replace_tilde_with_home_path(string &str, const string &home_path);
void parse_argv(std::unordered_map<string, string> &key_val, int argc,
                char **argv);
} /* namespace Setting_util */
} /* namespace Teave */
#endif /* TEAVE_SETTING_UTIL_H */