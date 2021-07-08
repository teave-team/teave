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
#ifndef TEAVE_RUNTIME_H
#define TEAVE_RUNTIME_H

#include <teave/err/err.h>
#include <teave/user/user.h>
#include <string>
#include <memory>

namespace Teave {

class Runtime {
public:
    using uptr = std::unique_ptr<Runtime>;
    using string = std::string;

    Runtime(Err *const err, User *const user);
    /* this is static because we need this method
    before User is created (see ctor param) */
    static string get_teave_sv_socket_path();
    string get_runtime_dir();

private:
    Err *const err;

    User *const user;

    string runtime_dir;

    void create_runtime_dir();
};

} // namespace Teave

#endif /* TEAVE_RUNTIME_H */
