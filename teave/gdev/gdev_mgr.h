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
#ifndef GDEV_MGR_H
#define GDEV_MGR_H

#include <teave/epoll/epoll_mgr.h>
#include <teave/gdev/drm.h>
#include <teave/event/event.h>
#include <vector>
#include <teave/setting/settings.h>
#include <teave/sv/sva.h>

namespace Teave {

#define MAX_GDEV_CONS 20

/* Graphics Device Manager */
class Gdev_mgr {
public:
    using uptr = std::unique_ptr<Gdev_mgr>;

    std::vector<Gdev::uptr> devs;

    Gdev_mgr(Err *const err, Settings *const settings,
             Epoll_mgr *const epoll_mgr, Sva *const sva);

    ~Gdev_mgr();

    void process_event(Epoll_reg *ep_reg);

    void schedule_vblank(Gcon *con);

    int get_num_cons();

    /* for debuging purposes */
    void list_info();

private:
    Err *const err = nullptr;
    Settings *const settings;
    Epoll_mgr *const epoll_mgr = nullptr;
    Sva *const sva = nullptr;

    drmEventContext drm_ec; /* for registering page flip handler */

    int num_cons = 0; // number of gdev connectors

    Udev_w udev_w;

    void load_devs();

    void load_num_cons();

    void setup_dev(Gdev *dev, int& con_id);

    bool is_crtc_in_use(uint32_t crtc_id);
};

} // namespace Teave
#endif /* GDEV_MGR_H */