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
#include <teave/device/udev.h>
#include <unistd.h>

namespace Teave {

Dev_f::Dev_f(string &path) : path(path) {}

Dev_f::Dev_f(const char *path) : path(path) {}

Dev_f::~Dev_f() {
    if (fd > -1)
        close(fd);
}

Udev_dev::Udev_dev(udev_device *ud_dev) : dev(ud_dev) {}
Udev_dev::~Udev_dev() {
    if (dev)
        udev_device_unref(dev);
}

Udev_enum::Udev_enum(struct udev *ud) {
    if (!ud)
        throw Run_err("ud is null", TE_ERR_LOC);
    en = udev_enumerate_new(ud);
    if (!en)
        throw Run_err("udev_enumerate_new failed", TE_ERR_LOC);
}

Udev_enum::~Udev_enum() {
    if (en)
        udev_enumerate_unref(en);
}

struct udev_list_entry *Udev_enum::get_list(const char *sub_sys,
                                            const char *sys_name) {
    int ret = udev_enumerate_add_match_subsystem(en, sub_sys);
    if (ret < 0)
        throw Sys_err(-ret,
                      "udev_enumerate_add_match_subsystem err, sub_sys: " +
                          string(sub_sys),
                      TE_ERR_LOC);

    ret = udev_enumerate_add_match_sysname(en, sys_name);
    if (ret < 0)
        throw Sys_err(-ret,
                      "udev_enumerate_add_match_sysname err, sys_name: " +
                          string(sys_name),
                      TE_ERR_LOC);

    ret = udev_enumerate_scan_devices(en);
    if (ret < 0)
        throw Sys_err(-ret,
                      "udev_enumerate_scan_devices err, " +
                          string(" sub_sys: ") + sub_sys +
                          " sys_name: " + sys_name,
                      TE_ERR_LOC);

    return udev_enumerate_get_list_entry(en);
}

Udev_w::Udev_w() {
    ud = udev_new();
    if (!ud)
        throw Run_err("udev_new failed", TE_ERR_LOC);
}

Udev_w::~Udev_w() {
    if (ud)
        udev_unref(ud);
}

} // namespace Teave