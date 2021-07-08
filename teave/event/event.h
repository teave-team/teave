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
#ifndef TEAVE_EVENT_H
#define TEAVE_EVENT_H

#include <memory>
namespace Teave {

/* Event loop action */
enum class Ev_loop_act { continue_, quit };

/* Event type */
enum class Ev_type {
    sig, /* signal */
    none
};

struct Sig_ev {
    using uptr = std::unique_ptr<Sig_ev>;
    Sig_ev(uint32_t signo);
    Ev_type get_type();

    uint32_t signo = 0;
};

} // namespace Teave

#endif /* TEAVE_EVENT_H */
