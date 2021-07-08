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
#include "epoll_mgr.h"
#include <algorithm>
#include <unistd.h>

using std::string;

namespace Teave {

Epoll_reg::Epoll_reg(int fd, std::function<void(Epoll_reg *const ep_reg)> fnc,
                     void *data)
    : fd(fd), fnc(fnc), data(data) {}

Epoll_mgr::Epoll_mgr(int max_num_events) : max_num_events(max_num_events) {

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0)
        throw Sys_err(errno, "epoll_create1 failed", TE_ERR_LOC);

    if (max_num_events < 1)
        max_num_events = 64;

    events = new epoll_event[max_num_events];
    if (!events)
        throw Run_err("failed to create events", TE_ERR_LOC);
}

Epoll_mgr::~Epoll_mgr() {
    for (auto &cb : ep_regs)
        del_from_epoll_ctl(cb.first);

    ep_regs.clear();

    if (events)
        delete[] events;

    close(epoll_fd);
}

void Epoll_mgr::register_(Epoll_reg &&ep_reg, uint32_t epoll_events,
                          bool replace) {
    auto search = ep_regs.find(ep_reg.fd);
    if (search != ep_regs.end()) {
        if (replace)
            deregister(ep_reg.fd);
        else
            return;
    }

    auto ret = ep_regs.insert(std::make_pair(ep_reg.fd, ep_reg));
    if (!std::get<1>(ret))
        throw Run_err("fd_cbs.insert failed, fd: " + std::to_string(ep_reg.fd),
                      TE_ERR_LOC);

    struct epoll_event ev = {.events = epoll_events};

    ev.data.ptr = nullptr;
    auto it = ep_regs.find(ep_reg.fd);
    if (it != ep_regs.end())
        ev.data.ptr = &((*it).second);

    if (!ev.data.ptr)
        throw Run_err("failed to find newly inserted ep_reg, fd: " +
                          std::to_string(ep_reg.fd),
                      TE_ERR_LOC);

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ep_reg.fd, &ev) < 0) {
        deregister(ep_reg.fd);
        throw Sys_err(
            errno, "epoll_ctl failed to add fd: " + std::to_string(ep_reg.fd),
            TE_ERR_LOC);
    }
}

void Epoll_mgr::deregister(int fd) {

    del_from_epoll_ctl(fd);

    ep_regs.erase(fd);
}

void Epoll_mgr::del_from_epoll_ctl(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int Epoll_mgr::get_epoll_fd() { return epoll_fd; }

int Epoll_mgr::get_max_num_events() { return max_num_events; }

} // namespace Teave