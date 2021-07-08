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
#include <teave/runtime/runtime.h>
#include <vector>
#include <sys/stat.h>
#include <teave/err/err.h>
#include <unistd.h>
#include <fcntl.h>

using std::string;
using std::vector;

namespace Teave {

Runtime::Runtime(Err *const err, User *const user) : err(err), user(user) {
    create_runtime_dir();
}

string Runtime::get_teave_sv_socket_path() {
    vector<string> vec = {"run", "teave"};
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    string dir = {};
    for (auto &str : vec) {
        dir += "/" + str;
        if (mkdir(dir.c_str(), mode) < 0) {
            if (errno != EEXIST)
                throw Sys_err(
                    errno,
                    "system_runtime_dir: " + dir +
                        " doesn't exist and we were not able to create it",
                    TE_ERR_LOC);
        }
    }
    return dir + "/teave-sv.sock";
}

void Runtime::create_runtime_dir() {
    int uid = user->get_id();
    int gid = user->get_gid();

    vector<string> vec = {"run", "user", std::to_string(uid)};
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    string dir = {};
    for (auto &str : vec) {
        dir += "/" + str;
        if (mkdir(dir.c_str(), mode) < 0) {
            if (errno != EEXIST)
                throw Sys_err(
                    errno,
                    "system_runtime_dir: " + dir +
                        " doesn't exist and we were not able to create it",
                    TE_ERR_LOC);
        }
    }

    dir += "/teave";
    if (mkdir(dir.c_str(), mode) < 0) {
        if (errno != EEXIST)
            throw Sys_err(
                errno,
                "teave-runtime_dir: " + dir +
                    " doesn't exist and we were not able to create it",
                TE_ERR_LOC);
    }

    if (chown(dir.c_str(), uid, gid) < 0)
        throw Sys_err(errno,
                      "chown err, teave-runtime_dir: " + dir +
                          " to user_id: " + std::to_string(uid) +
                          " group_id: " + std::to_string(gid),
                      TE_ERR_LOC);

    /* note S_ISGID set-group-ID bit */
    if (chmod(dir.c_str(), mode | S_ISGID) < 0)
        throw Sys_err(errno,
                      "chmod err, teave-runtime_dir: " + dir +
                          " to user_id: " + std::to_string(uid) +
                          " group_id: " + std::to_string(gid),
                      TE_ERR_LOC);

    runtime_dir = dir;
}

string Runtime::get_runtime_dir() { return runtime_dir; }

} // namespace Teave