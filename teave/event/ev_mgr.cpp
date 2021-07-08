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
#include <teave/event/ev_mgr.h>

namespace Teave {

Ev_type Ev_mgr::ev_type = Ev_type::none;
Ev_loop_act Ev_mgr::ev_loop_act = Ev_loop_act::continue_;

Ev_mgr::Ev_mgr() { reset_indices(); }

void Ev_mgr::set_ev_type(Ev_type ev_type_) { Ev_mgr::ev_type = ev_type_; }

Ev_type Ev_mgr::get_ev_type() { return Ev_mgr::ev_type; }

void Ev_mgr::set_ev_loop_act(Ev_loop_act act) { Ev_mgr::ev_loop_act = act; }

Ev_loop_act Ev_mgr::get_ev_loop_act() { return Ev_mgr::ev_loop_act; }

string Ev_mgr::get_ev_loop_act_str(Ev_loop_act ev_loop_act) {
    switch (ev_loop_act) {
    case Ev_loop_act::continue_:
        return "continue_";
    case Ev_loop_act::quit:
        return "quit";
    }

    return ""; // to avoid compiler -Wreturn-type warning
}

void Ev_mgr::reset() { Ev_mgr::ev_type = Ev_type::none; }

void Ev_mgr::reset_indices() {
    cur_ev_i = -1;
    last_ev_i = -1;
}

} // namespace Teave