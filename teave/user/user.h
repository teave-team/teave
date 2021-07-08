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
#ifndef TEAVE_USER_H
#define TEAVE_USER_H

#include <teave/err/err.h>
#include <string>
#include <memory>

namespace Teave {

class User {
public:
    using uptr = std::unique_ptr<User>;
    using string = std::string;

    User();

    int get_id() const;
    int get_gid() const;
    string get_uname() const;
    string get_dir() const;
    void own_file(string &&path) const;

private:
    int id = -1;
    int gid = -1;
    string uname;
    string dir;
};

} // namespace Teave

#endif /* TEAVE_USER_H */
