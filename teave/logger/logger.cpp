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
#include "logger.h"
#include <teave/util/file.h>
#include <teave/util/time.h>
#include <teave/err/err.h>
#include <iostream>
#include <algorithm>
#include <teave/setting/prepro.h>

using namespace Teave::Fs;
using std::string;
namespace Teave {

Logger::Logger(string &file_path)
    : dests(Prepro::get_log_dest()), log_lvl(Prepro::get_log_lvl()),
      file_path(file_path) {
    init();
}
Logger::Logger(string &&file_path)
    : dests(Prepro::get_log_dest()), log_lvl(Prepro::get_log_lvl()),
      file_path(file_path) {
    init();
}
Logger::Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl)
    : dests(dests), log_lvl(log_lvl), file_path("") {
    init();
}

Logger::Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl, string &file_path)
    : dests(dests), log_lvl(log_lvl), file_path(file_path) {
    init();
}

Logger::Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl,
               string &&file_path)
    : dests(dests), log_lvl(log_lvl), file_path(std::move(file_path)) {
    init();
}

Logger::Logger(std::vector<Log_dest> &&dests, Log_lvl log_lvl,
               string &&file_path)
    : dests(std::move(dests)), log_lvl(log_lvl),
      file_path(std::move(file_path)) {
    init();
}

void Logger::set_file_path(string &file_path_) {
    file_path = file_path_;
    process_dest();
}
string Logger::get_file_path() { return file_path; }

void Logger::set_log_lvl(Log_lvl lvl) { log_lvl = lvl; }
Log_lvl Logger::get_log_lvl() { return log_lvl; }

void Logger::init() {
    /* process log level */
    set_log_lvl(Prepro::get_log_lvl());
    process_dest();
}

void Logger::process_dest() {
    for (auto dest : dests) {
        switch (dest) {
        case Log_dest::file:
            if (file_path.empty())
                throw Run_err(
                    "Log_dest::file is provided but file_path is empty",
                    TE_ERR_LOC);
            break;
        case Log_dest::syslog:
            syslog_f = true;
            break;
        default:
            break;
        }
    }
}

void Logger::log(string msg, bool timestamp_flag, int syslog_priority) const {
    if (timestamp_flag)
        msg = std::to_string(get_time_milliseconds().count()) + " " + msg;

    for (auto &dest : dests) {
        switch (dest) {
        /* changes to here must refelect in settings.cpp and settings.txt as
         * well */
        case Log_dest::file:
            write_to_file(file_path, msg);
            break;
        case Log_dest::stderr:
            std::cerr << msg << std::endl;
            break;
        case Log_dest::stdout:
            std::cout << msg << std::endl;
            break;
        case Log_dest::syslog:
            if (syslog_priority == -1)
                syslog_priority = LOG_INFO;
            syslog(syslog_priority, "%s", msg.c_str());
            break;
        case Log_dest::none:
            break;
        }
    }
}

void Logger::log(string msg, string origin, bool timestamp_flag,
                 int syslog_priority) const {
    /* see note in string Logger::log */
    log(origin + ": " + msg, timestamp_flag, syslog_priority);
}

} // namespace Teave
