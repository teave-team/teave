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
#include <teave/event/signal_handler.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>

using std::string;
/*
 * there hare to sig handlers:
 * 1. void sig_handler(int signo), below is called
 * when signals are not epolled (prior to registering with epoll
 * and after epoll_mgr is destroyed)
 * 2. in Teave-sv there is a sig handler
 * after signals are epolled.
 * The App's default setup like Teave-sv
 */
void sig_handler(int signo) {
    struct signalfd_siginfo sig;
    sig.ssi_signo = signo;
    Teave::process_signal(sig);
}

namespace Teave {

/*
 * THESE STATIC VARS ARE ONLY USED FOR SIGNAL PROCESSING
 */
static bool we_had_fatal_sig = false;
/* enum Process Type, see common.h */
static Proc_type gs_proc_type = PROC_APP;
static Teave::Err *gs_err = nullptr;
/* cleanup callback */
static std::function<void(signalfd_siginfo &sig)> gs_sig_cb = nullptr;

void setup_sig_globals(Proc_type proc_type, Err *err_,
                       std::function<void(signalfd_siginfo &sig)> cb) {
    gs_proc_type = proc_type;
    gs_err = err_;
    gs_sig_cb = cb;
}

/*
 * how: SIG_BLOCK or SIG_UNBLOCK
 * see: http://man7.org/linux/man-pages/man2/sigprocmask.2.html
 * http://man7.org/linux/man-pages/man7/signal.7.html
 */

void setup_signals(Sig_set_act act) {
    int fd = -1;
    setup_signals(act, fd);
}

void setup_signals(Sig_set_act act, int &fd) {
    sigset_t set;
    /* set everything on bit mask */
    if (sigfillset(&set) < 0)
        throw Run_err("sigfillset(&set) err", TE_ERR_LOC);

    if (act == Sig_set_act::handler_only) {
        struct sigaction sig_act;
        sig_act.sa_mask = set;
        sig_act.sa_handler = sig_handler;
        if (sigaction(SIGABRT, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGABRT err", TE_ERR_LOC);
        if (sigaction(SIGBUS, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGBUS err", TE_ERR_LOC);
        if (sigaction(SIGHUP, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGHUP err", TE_ERR_LOC);
        if (sigaction(SIGINT, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGINT err", TE_ERR_LOC);
        if (sigaction(SIGQUIT, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGQUIT err", TE_ERR_LOC);
        if (sigaction(SIGTERM, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGTERM err", TE_ERR_LOC);
        if (sigaction(SIGTSTP, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGTSTP err", TE_ERR_LOC);
        if (sigaction(SIGSEGV, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGSEGV err", TE_ERR_LOC);
        if (sigaction(SIGILL, &sig_act, NULL) < 0)
            throw Sys_err(errno, "sigaction SIGILL err", TE_ERR_LOC);

    } else if (act == Sig_set_act::block) {
        if (sigprocmask(SIG_BLOCK, &set, NULL) < 0)
            throw Sys_err(errno, "sigprocmask SIG_BLOCK err", TE_ERR_LOC);

        fd = signalfd(-1, &set, SFD_NONBLOCK | SFD_CLOEXEC);
        if (fd < 0)
            throw Sys_err(errno, "signalfd err", TE_ERR_LOC);

    } else {
        if (sigprocmask(SIG_UNBLOCK, &set, NULL) < 0)
            throw Sys_err(errno, "sigprocmask SIG_UNBLOCK err", TE_ERR_LOC);
    }
}

/**
 * called from process_signal_event
 * param string info: caller can send additional info
 * for to see in the logs in case of a singal
 */
void process_signal(signalfd_siginfo &sig) {
    switch (sig.ssi_signo) {
    case SIGCHLD:
        /*
         * caused by termination of App's sub processes, we ignore and return
         */
        if (gs_proc_type == PROC_APP) {
            /* in case of multiple signals, see
             * http://man7.org/linux/man-pages/man7/signal.7.html
             * re multiple signals
             */
            pid_t pid;
            while (1) {
                pid = waitpid(-1, NULL, WNOHANG | WUNTRACED | WCONTINUED);
                if (pid <= 0)
                    break;
            }
            return;
        }
        break;

    case SIGABRT:
    case SIGBUS:
    case SIGSEGV:
    case SIGILL:
        if (gs_err)
            gs_err->log(string(" sig: ") + std::to_string(sig.ssi_signo),
                        TE_ERR_LOC);
        backtrace_();

    case SIGHUP:
    case SIGINT: {
        /* to avoid infinite loops of sigs
         * in case we get in a loop of
         * trying to quit because of a signal
         * but getting new singals
         */
        if (we_had_fatal_sig)
            std::exit(EXIT_FAILURE);
        else
            we_had_fatal_sig = true;

        /* call cleanup callback */
        gs_sig_cb(sig);
        Ev_mgr::set_ev_loop_act(Ev_loop_act::quit);
        Ev_mgr::set_ev_type(Ev_type::sig);

        if (gs_err)
            gs_err->log(
                "Proc_type: " + std::to_string(gs_proc_type) +
                    " processed signal: " + std::to_string(sig.ssi_signo),
                TE_ERR_LOC);
        else
            throw Run_err("Proc_type: " + std::to_string(gs_proc_type) +
                              " (throw) processed signal: " +
                              std::to_string(sig.ssi_signo),
                          TE_ERR_LOC);
    } break;
    case SIGQUIT:
    case SIGTSTP:
    case SIGTERM:
        Ev_mgr::set_ev_loop_act(Ev_loop_act::quit);
        Ev_mgr::set_ev_type(Ev_type::sig);
        break;

    default: /* ignore other signals */
        return;
    };
}

/*
 * ref: http://man7.org/linux/man-pages/man3/backtrace.3.html
 */
void backtrace_() {
    if (!gs_err)
        return;

    int buf_size = 100;
    void *buffer[buf_size];
    int ret_size = backtrace(buffer, buf_size);
    char **strings = backtrace_symbols(buffer, ret_size);
    if (strings == NULL) {
        gs_err->log("no backtrace available", TE_ERR_LOC);
        return;
    }

    gs_err->log(" backtrace: ", "");
    for (int i = 0; i < ret_size; i++)
        gs_err->log(strings[i], "");
    gs_err->log(" backtrace end.", "");

    free(strings);
}

} // namespace Teave