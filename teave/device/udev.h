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
#ifndef TEAVE_UDEV_H
#define TEAVE_UDEV_H

#include <libudev.h>
#include <teave/err/err.h>
#include <memory>

namespace Teave {

using std::string;

/* device on file system */
struct Dev_f {
    using uptr = std::unique_ptr<Dev_f>;
    Dev_f(string &path);
    Dev_f(const char *path);
    ~Dev_f();

    string path; /* udev node_path, ex:
                    /sys/devices/pci0000:00/0000:00:02.0/drm/card0 */
    int fd = -1;
    uint32_t maj = 0;
    uint32_t min = 0;
};

struct Udev_dev {
    Udev_dev(udev_device *ud_dev);
    ~Udev_dev();
    udev_device *dev = nullptr;
};

struct Udev_enum {
    Udev_enum(struct udev *ud);
    ~Udev_enum();
    struct udev_enumerate *en;
    /*
     * return a pointer to the first entry of the list
     * see:
     * https://www.freedesktop.org/software/systemd/man/udev_enumerate_scan_devices.html
     */
    struct udev_list_entry *get_list(const char *sub_sys, const char *sys_name);
};

/* udev wrapper */
struct Udev_w {
    Udev_w();
    ~Udev_w();

    struct udev *ud = nullptr;
};

} // namespace Teave

#endif /* TEAVE_UDEV_H */
