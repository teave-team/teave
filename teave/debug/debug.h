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
#ifndef TEAVE_DEBUG_H
#define TEAVE_DEBUG_H

#include <teave/err/err.h>

namespace Teave {

struct Debug {
    static void setup(const Err *err);
    static void log(std::string msg, std::string origin);
};

} // namespace Teave

#endif /* TEAVE_DEBUG_H */
