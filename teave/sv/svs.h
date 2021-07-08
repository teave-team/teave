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
#ifndef TEAVE_SVS_H
#define TEAVE_SVS_H

#include <teave/sv/sv.h>

namespace Teave {
/* for Teave-sv */
class Svs : public Sv {
public:
    using uptr = std::unique_ptr<Svs>;
    Svs(Err *const err, Socket *socket, int accept_fd);
    void send_fd(int msg_fd, Msg &msg, int flags = 0);
};
} /* namespace Teave */
#endif /* TEAVE_SVS_H */