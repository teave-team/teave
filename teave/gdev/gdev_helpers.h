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
#ifndef TEAVE_GDEV_HELPERS_H
#define TEAVE_GDEV_HELPERS_H

#include <teave/gdev/gdev.h>

namespace Teave {

std::string gdev_info(Gdev *dev);
std::string gdev_con_info(Gcon *con);
void load_dev_cons(Err *err, std::vector<Gdev::uptr> &devs, Gdev *dev,
                   drmModeRes *res);
int32_t get_crtc(std::vector<Gdev::uptr> &devs, Gdev *dev, drmModeRes *res,
                 drmModeConnector *con);
bool is_crtc_in_use(std::vector<Gdev::uptr> &devs, uint32_t crtc_id);
void restore_saved_crtc(Gdev *dev, Gcon *con);

} // namespace Teave

#endif /* TEAVE_GDEV_HELPERS_H */
