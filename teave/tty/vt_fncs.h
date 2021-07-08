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
#ifndef TEAVE_VT_FNCS_H
#define TEAVE_VT_FNCS_H

#include <teave/err/err.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <signal.h>

/* virtual terminal switch in and out
 * see Teave::get_vt_mode
 */
#ifndef SIG_VT_SWITCH
#define SIG_VT_SWITCH SIGUSR1
#endif

#ifndef KDSKBMUTE
#define KDSKBMUTE 0x4B51
#endif

namespace Teave {
namespace Vt_fncs {

enum class Mode_type { auto_, sigs };

void set_vt_mode(int fd, vt_mode &mode, Err *const err);

void get_vt_mode(int fd, vt_mode &mode);

vt_mode create_vt_mode_struct(Mode_type mode_type, short relsig, short acqsig);

void set_kd_kb_mode(int fd, int kd_kb_mute, long kd_kb_mode, Err *const err);

void get_kd_kb_mode(int fd, long &kd_kb_mode);

void set_kd_mode(int fd, unsigned int kd_mode, Err *const err);

void get_kd_mode(int fd, unsigned int &kd_mode);

/*
 * at start and cleanup we wait (wait=true), however for user base/mid process
 * vt switch we do not want to wait
 */
bool make_active(Err *const err, int fd, int num, bool wait);

/* get current active vt */
int get_cur_act_vt(int fd);

} // namespace Vt_fncs
} // namespace Teave

#endif /* TEAVE_VT_FNCS_H */
