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
#ifndef TEAVE_EPOLL_MGR_H
#define TEAVE_EPOLL_MGR_H

#include <teave/err/err.h>
#include <sys/epoll.h>
#include <unordered_map>
#include <tuple>
#include <functional>

namespace Teave {

/* struct to pass to Epoll_mgr::register_ to register an fd to epoll on */
struct Epoll_reg {
    using uptr = std::unique_ptr<Epoll_reg>;

    Epoll_reg(int fd, std::function<void(Epoll_reg *const ep_reg)> fnc,
              void *data = nullptr);

    int fd = -1; /* fd we epoll-ed on */

    /**
     * Callback function/method that is called when there is an event epolled
     * for fd
     */
    std::function<void(Epoll_reg *const ep_reg)> fnc = nullptr;

    void *data = nullptr;
};

class Epoll_mgr {
public:
    using uptr = std::unique_ptr<Epoll_mgr>;

    /*
     * array of struct epoll_event
     * we need pointer here because of call to epoll_wait where we send this
     * as arg
     */
    struct epoll_event *events = nullptr;

    /* if max_num_events < 1, it is set to 64 */
    Epoll_mgr(int max_num_events = 64);

    ~Epoll_mgr();

    void register_(Epoll_reg &&ep_reg, uint32_t epoll_events = EPOLLIN,
                   bool replace = false);

    void deregister(int fd);

    int get_epoll_fd();

    int get_max_num_events();

private:
    int epoll_fd = -1;
    int max_num_events = 0;
    /* <fd, Epoll_reg> */
    std::unordered_map<int, Epoll_reg> ep_regs;

    void del_from_epoll_ctl(int fd);
};

} // namespace Teave

#endif /* TEAVE_EPOLL_MGR_H */
