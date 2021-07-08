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
#ifndef TEAVE_SETTINGS_H
#define TEAVE_SETTINGS_H

#include <functional>
#include <fstream>
#include <memory>
#include <teave/user/user.h>
#include <teave/setting/setting_common.h>
#include <teave/setting/setting_int.h>

namespace Teave {

class Settings {
public:
    using uptr = std::unique_ptr<Settings>;
    Settings(Err *err, int argc, char **argv_, User *user);
    Settings(Settings const &) = delete;
    Settings &operator=(Settings const &) = delete;
    void register_(
        std::string &&key,
        std::function<void(std::string &key, std::vector<std::string> &keys,
                           std::string &value)>
            fnc,
        bool multi = false);
    void register_vec(
        std::vector<std::string> &&keys,
        std::function<void(std::string &key, std::vector<std::string> &keys,
                           std::string &value)>
            fnc,
        bool multi = false);
    void register_vec(
        std::vector<std::string> &keys,
        std::function<void(std::string &key, std::vector<std::string> &keys,
                           std::string &value)>
            fnc,
        bool multi = false);

    void process(bool throw_ = true);

    /* settings getters and setters */
    void set_mode(std::string &key, std::vector<std::string> &keys,
                  std::string &val);
    Mode get_mode();
    /* end getters and setters */

    /* check dependencies */
protected:
    void reg_cbs();
    Err *err = nullptr;
    int argc = -1;
    char **argv = nullptr;
    User *user = nullptr;

    /* c_key is the compound key, ex: key_level1.key_level2.key_level3 */
    /* callback map <c_key, callback function> */
    std::unordered_map<std::string, Setting_int::Cb> cb_map;

    /* settings */
    Mode mode = Mode::prod;
    /* end settings */
};

} /* namespace Teave */

#endif /* TEAVE_SETTINGS_H */
