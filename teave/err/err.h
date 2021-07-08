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
#ifndef TEAVE_ERR_H
#define TEAVE_ERR_H

#include <teave/err/exceptions.h>
#include <teave/logger/logger.h>

#define TE_ERR_LOC BOOST_CURRENT_FUNCTION

namespace Teave {

class Err {
public:
    using uptr = std::unique_ptr<Err>;
    Err(std::string &file_path);
    Err(std::string &&file_path);
    Err(Logger *logger);

    void log(std::string msg, std::string origin,
             int syslog_priority = LOG_INFO) const;
    void log(int errn, std::string msg, std::string origin,
             int syslog_priority = LOG_INFO) const;
    Logger *get_logger();

protected:
    Logger *logger = nullptr;
    Logger::uptr lgr;
};

} // namespace Teave

#endif /* TEAVE_ERR_H */
