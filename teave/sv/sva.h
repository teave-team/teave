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
#ifndef TEAVE_SVA_H
#define TEAVE_SVA_H

#include <teave/sv/sv.h>

namespace Teave {
/* for the main App containing the Compositor */
class Sva : public Sv {
public:
    using uptr = std::unique_ptr<Sva>;
    Sva(Err *const err, Socket *socket, int fd);
    /*!
     * @param path device path
     * @param open_flags if -1, then flags are set by Er, see teave-r Dev::Dev()
     *
     * These class block until the fd is received from Teave_r
     */
    /* <success/failure, reply msg, fd> */
    std::tuple<bool, Msg::uptr, int> req_gdev_fd(string &path,
                                                 int open_flags = -1);
    std::tuple<bool, Msg::uptr, int> req_idev_fd(string &path,
                                                 int open_flags = -1);
    std::tuple<bool, Msg::uptr, int> req_vt_fd(string &path,
                                               int open_flags = -1);
    std::tuple<bool, Msg::uptr, int> req_fd(string &path, int open_flags = -1);
    std::tuple<bool, Msg::uptr, int> req_fd(Msg &msg);
    std::tuple<ssize_t, Msg::uptr, int> recv_fd();
};
} /* namespace Teave */
#endif /* TEAVE_SVA_H */