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
#include <teave/debug/debug.h>
#include <iostream>

using std::string;

namespace Teave {

/*
 * these functions are intended for debuging only
 * limit access to this file only. we wrap it in debug_log functions
 */
static const Err *gs_err = nullptr;

void Debug::setup(const Err *err) { gs_err = err; }

void Debug::log(string msg, string origin) {
    if (gs_err)
        gs_err->log(msg, origin);
    else
        std::cerr << origin << ": " << msg << std::endl;
}

} // namespace Teave
