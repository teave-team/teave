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
#include <teave/err/err.h>
#include "teave/include/boost/stacktrace.hpp"
#include "teave/include/boost/filesystem.hpp"
#include <iostream>

namespace Teave {
using std::string;

Err::Err(std::string &file_path) {
    lgr = std::make_unique<Logger>(file_path);
    logger = lgr.get();
}
Err::Err(std::string &&file_path) : Err(file_path) {}

Err::Err(Logger *logger) : logger(logger) {}

void Err::log(string msg, string origin, int syslog_priority) const {
    logger->log(msg, origin, true, syslog_priority);
}

void Err::log(int errn, string msg, string origin, int syslog_priority) const {

    logger->log(msg + ", errno " + std::to_string(errn), origin, true,
                syslog_priority);
}

Logger *Err::get_logger() { return logger; }
} // namespace Teave