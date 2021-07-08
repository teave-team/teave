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
#include <teave/tty/vt_fncs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

namespace Teave {
namespace Vt_fncs {

void set_vt_mode(int fd, vt_mode &mode, Err *const err) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    if (ioctl(fd, VT_SETMODE, &mode) < 0)
        err->log(errno, "ioctl VT_SETMODE err ", TE_ERR_LOC);
}
void get_vt_mode(int fd, vt_mode &mode) {
    if (ioctl(fd, VT_GETMODE, &mode) < 0)
        throw Sys_err(errno, "ioctl VT_GETMODE err", TE_ERR_LOC);
}

vt_mode create_vt_mode_struct(Mode_type mode_type, short relsig, short acqsig) {
    vt_mode mode;
    std::memset(&mode, 0, sizeof(vt_mode));

    switch (mode_type) {
    case Mode_type::auto_:
        mode.mode = VT_AUTO;
        break;
    case Mode_type::sigs:
        mode.mode = VT_PROCESS;
        mode.relsig = relsig;
        mode.acqsig = acqsig;
        break;
    }

    return mode;
}

void set_kd_kb_mode(int fd, int kd_kb_mute, long kd_kb_mode, Err *const err) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    /* see Tty::set_mode for more info re KDSKBMUTE and KDSKBMODE */
    if (ioctl(fd, KDSKBMUTE, kd_kb_mute) < 0 &&
        ioctl(fd, KDSKBMODE, kd_kb_mode) < 0)
        err->log(errno, "ioctl KDSKBMUTE 0 OR KDSKBMODE org_vt_kd_kb_mode err",
                 TE_ERR_LOC);
}

void get_kd_kb_mode(int fd, long &kd_kb_mode) {
    if (ioctl(fd, KDGKBMODE, &kd_kb_mode) < 0)
        throw Sys_err(errno, "ioctl KDGKBMODE err", TE_ERR_LOC);
}

void set_kd_mode(int fd, unsigned int kd_mode, Err *const err) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    if (ioctl(fd, KDSETMODE, kd_mode) < 0)
        err->log(errno,
                 "ioctl KDSETMODE kd_mode err, kd_mode: " +
                     std::to_string(kd_mode),
                 TE_ERR_LOC);
}
void get_kd_mode(int fd, unsigned int &kd_mode) {
    if (ioctl(fd, KDGETMODE, &kd_mode) < 0)
        throw Sys_err(errno, "ioctl KDGETMODE err", TE_ERR_LOC);
}

bool make_active(Err *const err, int fd, int num, bool wait) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);

    if (ioctl(fd, VT_ACTIVATE, num) < 0) {
        err->log(errno,
                 "ioctl VT_ACTIVATE vt_num err, num: " + std::to_string(num),
                 TE_ERR_LOC);
        return false;
    }

    if (wait) {
        if (ioctl(fd, VT_WAITACTIVE, num) < 0) {
            err->log(errno,
                     "ioctl VT_WAITACTIVE vt_num err, num: " +
                         std::to_string(num),
                     TE_ERR_LOC);
            return false;
        }
    }

    return true;
}

int get_cur_act_vt(int fd) {
    struct vt_stat state;
    if (ioctl(fd, VT_GETSTATE, &state) < 0)
        throw Sys_err(errno, "ioctl VT_GETSTATE err", TE_ERR_LOC);
    return state.v_active;
}

} // namespace Vt_fncs
} // namespace Teave