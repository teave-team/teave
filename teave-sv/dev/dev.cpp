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
#include "teave-sv/dev/dev.h"
#include <unistd.h>
#include <fcntl.h>
#include <xf86drm.h>
#include <teave/gdev/drm.h>

namespace Te = Teave;

namespace Teave_sv {

Dev::Dev(Te::Err *err, string &path, Type type, int open_flags)
    : err(err), path(path), type(type) {
    if (!err)
        throw Te::Arg_err("err is null", TE_ERR_LOC);
    if (open_flags < 0)
        open_flags = O_RDWR | O_CLOEXEC;
    fd = open(path.c_str(), open_flags);
    if (fd < 0)
        throw Te::Sys_err(errno, "open err, path: " + path, TE_ERR_LOC);
}

Dev::~Dev() { close(fd); }

Gdev::Gdev(Te::Err *err, string &path, int open_flags)
    : Dev(err, path, Type::gdev, open_flags) {
    if (drmSetMaster(fd) < 0)
        throw Te::Sys_err(errno, "drmSetMaster err, path: " + path +
                                     " fd: " + std::to_string(fd));
}

Gdev::~Gdev() {
    if (drmDropMaster(fd) < 0)
        err->log(errno,
                 "drmDropMaster err, path: " + path +
                     " fd: " + std::to_string(fd),
                 TE_ERR_LOC);
    close(fd);
}

} // namespace Teave_sv