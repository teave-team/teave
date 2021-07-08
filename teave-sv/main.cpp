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
#include "teave-sv/mgr/mgr.h"
#include <syslog.h>

int main(int argc, char *argv[]) {
    openlog("teave-sv", LOG_CONS | LOG_NDELAY | LOG_PERROR, LOG_DAEMON);

    try {
        Teave_sv::Mgr mgr(argc, argv);
        return mgr.run();

    } catch (std::exception &e) {
        syslog(LOG_ERR, "%s", e.what());
        std::exit(EXIT_FAILURE);
    }

    return 0;
}