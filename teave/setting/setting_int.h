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
#ifndef TEAVE_SETTING_INT_H
#define TEAVE_SETTING_INT_H

#include <vector>
#include <string>
#include <functional>

namespace Teave {
/* setting internals */
namespace Setting_int {
using std::string;
/*
    ex. of key/val for demonstrating the following: struct Cb and cb_map
    key_level1 = {
        key_level2 = {
            key_level3 = val;
        }
    }
    */
/* c_key is the compound key, ex: key_level1.key_level2.key_level3 */
struct Cb {
    std::vector<string>
        keys; /* key from each level: key_level1, key_level2, key_level3 */
    std::function<void(string &c_key, std::vector<string> &keys, string &value)>
        fnc;
    bool multi = false;  /* used for multiple keys */
    bool called = false; /* flag to ensure cb has been called at least once when
                            multi is true */
};

struct Nf {
    std::vector<string>
        keys; /* key from each level: key_level1, key_level2, key_level3 */
    string val;
};
} /* namespace Setting_int */
} /* namespace Teave */

#endif /* TEAVE_SETTING_INT_H */
