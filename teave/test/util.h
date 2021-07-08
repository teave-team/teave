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
#ifndef TEAVE_TEST_UTIL_H
#define TEAVE_TEST_UTIL_H

#include <vector>
#include <string>
#include <unordered_map>
#include <signal.h>
#include <teave/err/err.h>

/* by convention we SIGUSR1 to signal quit normally */
#define EIT_QUIT_SIG SIGUSR1

namespace Teave {
using std::string;

namespace Test {
/*!
 * if env TEAVE_TEST_TMP_DIR is set and not empty, it's returned
 * if not, ../test/tmp is returned
 */
string get_test_tmp_dir();
string get_test_log_file();
Err::uptr create_err_obj(string &log_path);
std::vector<string> get_file_content(string &path);
string umap_str_to_str(std::unordered_map<string, string> &map);

/*!
 * @return process id of the exec process
 */
int exec_proc(const char *path, char *argv[], char *envp[]);
/*!
 * @exception throws Run_err if an error other than ESRCH is raised
 * @note by convention we use SIGUSR1 for test, to singnal a normal quit
 * for an example see teave/test/test_er_cred.cpp
 */
void term_proc(pid_t pid, int sig);

/*!
 * @exception throws Sys_err in case of err
 */
void set_env(const char *name, const char *value);

std::vector<char *> str_vec_to_char_ptr_vec(std::vector<string> &vec);

} // namespace Test
} // namespace Teave

#endif /* TEAVE_TEST_UTIL_H */