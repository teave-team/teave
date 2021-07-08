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
#ifndef TEAVE_DRM_H
#define TEAVE_DRM_H

#include <teave/gdev/gdev.h>

/**
 * will be called by the linux drm system, we'll put it outside of our namespace
 */
void te_vblank_handler(int fd, unsigned int sequence, unsigned int tv_sec,
                       unsigned int tv_usec, void *user_data);

namespace Teave {

struct Drm_mode_res {
    Drm_mode_res(int fd, string &path);
    ~Drm_mode_res();

    drmModeRes *res = nullptr;
};

struct Drm_enc {
    ~Drm_enc();

    drmModeEncoder *enc = nullptr;
};

struct Drm_con {
    Drm_con(int fd, uint32_t con_id);

    ~Drm_con();

    drmModeConnector *con = nullptr;
};

} // namespace Teave

#endif /* TEAVE_DRM_H */
