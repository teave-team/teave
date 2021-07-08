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
#ifndef TEAVE_PREPRO_H
#define TEAVE_PREPRO_H

#include <teave/logger/logger_common.h>
#include <teave/setting/setting_common.h>
#include <string>
#include <vector>

namespace Teave {
/* getters for the preprocessors set in CMake */
namespace Prepro {
std::vector<Log_dest> get_log_dest();
Log_lvl get_log_lvl();
std::string get_log_file();
std::string get_sv_log_file();
std::string get_test_log_file();
/* apps should use settings::get_mode()
instead of this function */
Mode get_mode();

} /* namespace Prepro */
} /* namespace Teave */
#endif /* TEAVE_PREPRO_H */