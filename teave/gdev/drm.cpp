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
#include <teave/gdev/drm.h>
#include <teave/err/err.h>
#include <teave/compositor/compositor.h>
#include <teave/util/util.h>

namespace Te = Teave;

void te_vblank_handler(int fd, unsigned int sequence, unsigned int tv_sec,
                       unsigned int tv_usec, void *user_data) {
    Te::Gcon *con = reinterpret_cast<Te::Gcon *>(user_data);
    con->comp->process_vblank(con);
}

namespace Teave {

Drm_mode_res::Drm_mode_res(int fd, string &path) {
    res = drmModeGetResources(fd);
    if (!res)
        throw Sys_err(errno,
                      "drmModeGetResources err, fd: " + std::to_string(fd) +
                          ", path: " + path,
                      TE_ERR_LOC);
}

Drm_mode_res::~Drm_mode_res() { drmModeFreeResources(res); }

Drm_enc::~Drm_enc() {
    if (enc)
        drmModeFreeEncoder(enc);
}

Drm_con::Drm_con(int fd, uint32_t con_id) {
    con = drmModeGetConnector(fd, con_id);
}

Drm_con::~Drm_con() {
    if (con)
        drmModeFreeConnector(con);
}

} // namespace Teave
