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
#include <teave/setting/setting_util.h>
#include "teave/include/boost/algorithm/string.hpp"

namespace Teave {
namespace Setting_util {

string get_parse_err_str(string &&msg, int line_num, string &line) {
    return "err: " + msg + ", line " + std::to_string(line_num) + ": " + line;
}

string get_err_msg(string &key, string &value) {
    return value + " is not a valid option for " + key;
}
string get_err_msg(string &&key, string &&value) {
    return value + " is not a valid option for " + key;
}

string get_err_msg_settings_depend(string &key1, string &value1, string &key2,
                                   string &value2) {
    return key1 + " cannot be " + value1 + " when " + value2 +
           " is provided as an option for " + key2;
}
string get_err_msg_settings_depend(string &&key1, string &&value1,
                                   string &&key2, string &&value2) {
    return key1 + " cannot be " + value1 + " when " + value2 +
           " is provided as an option for " + key2;
}

void replace_tilde_with_home_path(string &str, const string &home_path) {
    if (!str.empty()) {
        if (str.at(0) == '~')
            str.replace(0, 1, home_path);
    }
}

void parse_argv(std::unordered_map<string, string> &key_val, int argc,
                char **argv) {
    if (argv == NULL)
        return;
    if (argc < 2)
        return;
    int i = 1;
    while (argv[i] != NULL) {
        string tmp(argv[i]);
        tmp.erase(0, 2);
        i++;
        std::vector<string> kv;
        boost::split(kv, tmp, boost::is_any_of("="), boost::token_compress_off);
        if (kv.size() < 1)
            continue;
        if (kv.size() == 1)
            key_val[kv.front()] = "";
        else if (kv.size() == 2)
            key_val[kv.front()] = kv.back();
    }
}

} /* namespace Setting_util */
} /* namespace Teave */
