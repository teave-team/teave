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
#ifndef TEAVE_EV_MGR_H
#define TEAVE_EV_MGR_H

#include <teave/err/err.h>
#include <teave/event/event.h>
#include <array>
#include <string>

namespace Teave {
using std::string;

class Ev_mgr {
public:
    using uptr = std::unique_ptr<Ev_mgr>;
    /* return value of add_ev */
    enum class Add_ret { success, full };

    Ev_mgr();
    static void set_ev_type(Ev_type ev_type);
    static Ev_type get_ev_type();
    static void set_ev_loop_act(Ev_loop_act act);
    static Ev_loop_act get_ev_loop_act();
    string get_ev_loop_act_str(Ev_loop_act ev_loop_act);
    void reset();

private:
    static Ev_type ev_type;
    static Ev_loop_act ev_loop_act;

    /* for all array of events */
    int cur_ev_i = -1;  // current event index
    int last_ev_i = -1; // last inserted event index

    void reset_indices();
};

} // namespace Teave

#endif /* TEAVE_EV_MGR_H */
