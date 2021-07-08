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
#include <teave/setting/settings.h>
#include <teave/util/util.h>
#include <teave/setting/setting_util.h>
#include <teave/setting/prepro.h>
#include <teave/common.h>

using namespace std::placeholders;
using namespace Teave::Setting_util;
using namespace Teave::Setting_int;
namespace Teave {
Settings::Settings(Err *err, int argc, char **argv_, User *user)
    : err(err), argc(argc), argv(argv_), user(user) {
    reg_cbs();
}

void Settings::register_(
    string &&key,
    std::function<void(string &key, std::vector<string> &keys, string &value)>
        fnc,
    bool multi) {
    register_vec({key}, fnc, multi);
}

void Settings::register_vec(
    std::vector<string> &&keys,
    std::function<void(string &key, std::vector<string> &keys, string &value)>
        fnc,
    bool multi) {
    register_vec(keys, fnc, multi);
}

void Settings::register_vec(
    std::vector<string> &keys,
    std::function<void(string &key, std::vector<string> &keys, string &value)>
        fnc,
    bool multi) {
    std::pair<std::unordered_map<string, Cb>::iterator, bool> p;
    p = cb_map.insert_or_assign(Util::vec_str_to_str(keys), Cb());
    p.first->second.keys = keys;
    p.first->second.fnc = fnc;
    p.first->second.multi = multi;
}

void Settings::reg_cbs() {
    register_("mode", std::bind(&Settings::set_mode, this, _1, _2, _3));
}

void Settings::process(bool throw_) {
    string val = "";
    for (auto &cb : cb_map) {
        string k(cb.first);
        cb.second.fnc(k, cb.second.keys, val);
    }

    /* check dependencies */
}

/* getters and setters */

void Settings::set_mode(string &key, std::vector<string> &keys, string &val) {
    mode = Prepro::get_mode();
}
Mode Settings::get_mode() { return mode; }

} /* namespace Teave */