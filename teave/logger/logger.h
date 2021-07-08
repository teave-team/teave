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
#ifndef TEAVE_LOGGER_H
#define TEAVE_LOGGER_H

#include "teave/include/boost/current_function.hpp"
#include <string>
#include <vector>
#include <memory>
#include <syslog.h>
#include <teave/logger/logger_common.h>

/*
 * used as the origin of the log message (the function that generated the
 * log message)
 * ex: logger->log("my message std::string", TE_CUR_LOC);
 */
#define TE_CUR_LOC BOOST_CURRENT_FUNCTION

namespace Teave {

/* Logger */
class Logger {
public:
    using uptr = std::unique_ptr<Logger>;
    /* uses
    Prepro::get_log_dest()
    Prepro::get_log_lvl()
    */
    Logger(std::string &file_path);
    Logger(std::string &&file_path);
    Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl);
    Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl,
           std::string &file_path);
    Logger(std::vector<Log_dest> &dests, Log_lvl log_lvl,
           std::string &&file_path);
    Logger(std::vector<Log_dest> &&dests, Log_lvl log_lvl,
           std::string &&file_path);
    void set_file_path(std::string &file_path);
    std::string get_file_path();
    void set_log_lvl(Log_lvl lvl);
    Log_lvl get_log_lvl();

    void log(std::string msg, bool timestamp_flag = true,
             int syslog_priority = LOG_INFO) const;
    void log(std::string msg, std::string origin, bool timestamp_flag = true,
             int syslog_priority = LOG_INFO) const;

private:
    std::vector<Log_dest> dests;
    Log_lvl log_lvl = Log_lvl::err;
    std::string file_path;
    bool syslog_f = false; /* syslog flag */

    void init();
    void process_dest();
};

} // namespace Teave

#endif /* TEAVE_LOGGER_H */