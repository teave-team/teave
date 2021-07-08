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
#ifndef TEAVE_SIGNAL_HANDLER_H
#define TEAVE_SIGNAL_HANDLER_H

#include <teave/err/err.h>
#include <teave/event/ev_mgr.h>
#include <teave/common.h>
#include <functional>
#include <signal.h>
#include <sys/signalfd.h>

namespace Teave {

/* signal setup action */
enum class Sig_set_act {
    block,
    unblock,
    handler_only /* only register sigs we handle with handler, used for initial
                    signal setup */
};

/*!
 * sets vars for singal processing and logging.
 * note, callback (cb) is called after the singal has been
 * processed to do final cleanup
 */
void setup_sig_globals(Proc_type proc_type, Err *err_,
                       std::function<void(signalfd_siginfo &sig)> cb);
/* without epoll */
void setup_signals(Sig_set_act act);
/* with epoll */
void setup_signals(Sig_set_act act, int &fd);
void process_signal(signalfd_siginfo &sig);

void backtrace_();

} // namespace Teave

#endif /* TEAVE_SIGNAL_HANDLER_H */
