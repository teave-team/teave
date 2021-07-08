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
#ifndef TEAVE_TTY_H
#define TEAVE_TTY_H

#include <teave/err/err.h>
#include <teave/sv/sva.h>
#include <teave/tty/vt_fncs.h>

namespace Teave {

class Tty {
public:
    using uptr = std::unique_ptr<Tty>;
    Tty(Err *const err, Sva *const sva, string vt_path = "/dev/tty0");
    ~Tty();
    int get_run_vt_num();
    int get_fd();
    long get_org_vt_kd_kb_mode();
    unsigned int get_org_vt_kd_mode();
    struct vt_mode get_org_vt_mode();

protected:
    /*
     * save org settings to be able to revert back at cleanup
     */
    void load_org_vt_settings();

    Err *const err = nullptr;
    Sva *const sva = nullptr;
    int fd = -1;
    int vt_num = -1; /* virtual terminal we run in */

    long org_vt_kd_kb_mode = -1;
    unsigned int org_vt_kd_mode = KD_TEXT;
    struct vt_mode org_vt_mode;
};
} // namespace Teave

#endif /* TEAVE_TTY_H */
