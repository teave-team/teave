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
#ifndef TEAVE_LOGGER_COMMON_H
#define TEAVE_LOGGER_COMMON_H

namespace Teave {
/* Log destination */
enum class Log_dest { file, syslog, stderr, stdout, none };

/* Log level */
enum class Log_lvl {
    none, /* no logs */
    err,  /* only errors */
    info  /* errors and info */
};

} // namespace Teave

#endif /* TEAVE_LOGGER_COMMON_H */