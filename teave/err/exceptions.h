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
#ifndef TEAVE_EXCEPTIONS_H
#define TEAVE_EXCEPTIONS_H

#include <stdexcept>
#include <system_error>

namespace Teave {

class Arg_err : public std::invalid_argument {
public:
    Arg_err(std::string msg, std::string origin)
        : std::invalid_argument(origin + ": " + msg) {}
};

class Run_err : public std::runtime_error {
public:
    Run_err(std::string msg, std::string origin)
        : std::runtime_error(origin + ": " + msg) {}
    Run_err(std::string msg) : std::runtime_error(msg) {}
};

class Sys_err : public std::system_error {
public:
    Sys_err(int errn, std::string origin)
        : std::system_error(errn, std::generic_category(),
                            origin + ": " + "errno " + std::to_string(errn)) {}

    Sys_err(int errn, std::string msg, std::string origin)
        : std::system_error(errn, std::generic_category(),
                            origin + ": " + msg + ", errno " +
                                std::to_string(errn)) {}
};

} // namespace Teave

#endif /* TEAVE_EXCEPTIONS_H */