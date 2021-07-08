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
#include <teave/user/user.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>

namespace Teave {
using std::string;

User::User() {
    id = geteuid();
    struct passwd *pwd;

    pwd = getpwuid(id);
    if (!pwd)
        throw Sys_err(errno, "getpwuid err, id: " + std::to_string(id),
                      TE_ERR_LOC);

    gid = pwd->pw_gid;

    if (!pwd->pw_name)
        throw Run_err("pw_name is null", TE_ERR_LOC);
    uname = pwd->pw_name;

    if (!pwd->pw_dir)
        throw Run_err("pw_dir is null", TE_ERR_LOC);
    dir = pwd->pw_dir;
}

int User::get_id() const { return id; }

int User::get_gid() const { return gid; }

string User::get_uname() const { return uname; }

string User::get_dir() const { return dir; }

void User::own_file(string &&path) const {
    if (chown(path.c_str(), id, gid) < 0)
        throw Sys_err(errno,
                      "chown err, path: " + path +
                          " to user_id: " + std::to_string(id) +
                          " group_id: " + std::to_string(gid),
                      TE_ERR_LOC);
}

} // namespace Teave