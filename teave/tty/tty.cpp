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
#include <teave/tty/tty.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

using namespace Teave::Vt_fncs;
using std::string;

namespace Teave {

Tty::Tty(Err *const err, Sva *const sva, string vt_path) : err(err), sva(sva) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    bool ret = false;
    std::tie(ret, std::ignore, fd) = sva->req_vt_fd(vt_path);
    if (!ret)
        throw Sys_err(errno, "er_mgr->req_fd failed, vt_path: " + vt_path,
                      TE_ERR_LOC);
    vt_num = get_cur_act_vt(fd);
    load_org_vt_settings();

    if (!make_active(err, fd, vt_num, true))
        throw Run_err("vt_make_active failed", TE_ERR_LOC);

    vt_mode vt_m =
        create_vt_mode_struct(Mode_type::sigs, SIG_VT_SWITCH, SIG_VT_SWITCH);
    set_vt_mode(fd, vt_m, err);
    set_kd_kb_mode(fd, 1, K_OFF, err);
    set_kd_mode(fd, KD_GRAPHICS, err);
}

Tty::~Tty() {
    if (fd > -1) {
        set_vt_mode(fd, org_vt_mode, err);
        set_kd_kb_mode(fd, 0, org_vt_kd_kb_mode, err);
        set_kd_mode(fd, org_vt_kd_mode, err);
        close(fd);
    }
}
int Tty::get_run_vt_num() { return vt_num; }

int Tty::get_fd() { return fd; }

/* save original modes so that we can revert back */
void Tty::load_org_vt_settings() {
    get_kd_kb_mode(fd, org_vt_kd_kb_mode);
    get_kd_mode(fd, org_vt_kd_mode);
    get_vt_mode(fd, org_vt_mode);
}

long Tty::get_org_vt_kd_kb_mode() { return org_vt_kd_kb_mode; }
unsigned int Tty::get_org_vt_kd_mode() { return org_vt_kd_mode; }
vt_mode Tty::get_org_vt_mode() { return org_vt_mode; }

} // namespace Teave