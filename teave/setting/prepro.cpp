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
#include <teave/setting/prepro.h>
#include <teave/common.h>
#include <teave/util/util.h>

#define TE_LOG_DEST TE_MAKE_STR_1(TE_LOG_DEST_)
#define TE_LOG_LVL TE_MAKE_STR_1(TE_LOG_LVL_)
#define TE_LOG_FILE TE_MAKE_STR_1(TE_LOG_FILE_)
#define TE_SV_LOG_FILE TE_MAKE_STR_1(TE_SV_LOG_FILE_)
#define TE_TEST_LOG_FILE TE_MAKE_STR_1(TE_TEST_LOG_FILE_)
#define TE_SV_SOCKET TE_MAKE_STR_1(TE_SV_SOCKET_)
#define TE_MODE TE_MAKE_STR_1(TE_MODE_)

using std::string;
namespace Teave {
namespace Prepro {
std::vector<Log_dest> get_log_dest() {
    std::vector<Log_dest> ret;
    string te_log_dest(TE_LOG_DEST);
    std::vector<string> vec = Util::split_str(te_log_dest, "-");
    for (string &str : vec) {
        if (str == "file")
            ret.push_back(Log_dest::file);
        else if (str == "syslog")
            ret.push_back(Log_dest::syslog);
        else if (str == "stderr")
            ret.push_back(Log_dest::stderr);
        else if (str == "stdout")
            ret.push_back(Log_dest::stdout);
        else if (str == "none")
            ret.push_back(Log_dest::none);
    }
    return ret;
}

Log_lvl get_log_lvl() {
    string str(TE_LOG_LVL);
    if (str == "info")
        return Log_lvl::info;
    else if (str == "none")
        return Log_lvl::none;
    else
        return Log_lvl::err;
}

std::string get_log_file() { return TE_LOG_FILE; }

std::string get_sv_log_file() { return TE_SV_LOG_FILE; }

std::string get_test_log_file() { return TE_TEST_LOG_FILE; }

/* apps should use settings::get_mode()
instead of this function */
Mode get_mode() {
    string str(TE_MODE);
    if (str == "dev")
        return Mode::dev;
    else
        return Mode::prod;
}

} /* namespace Prepro */
} /* namespace Teave */