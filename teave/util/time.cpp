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
#include <teave/util/time.h>

namespace Teave {

std::chrono::milliseconds get_time_milliseconds() {
    std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
}

} // namespace Teave
