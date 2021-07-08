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
#include "util.h"
#include <fstream>
#include <teave/err/err.h>
#include <unistd.h>
#include <stdlib.h>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace Teave {
namespace Test {

string get_test_tmp_dir() {
    string dir;
    const char *env_p = getenv("TEAVE_TEST_TMP_DIR");
    if (env_p)
        dir = env_p;
    if (dir.empty())
        dir = "../test/tmp";

    return dir;
}

string get_test_log_file() {
    string path;
    const char *env_p = getenv("TEAVE_TEST_LOG_FILE");
    if (env_p)
        path = env_p;
    if (path.empty())
        path = "../test/tmp/log.txt";

    return path;
}

std::vector<string> get_file_content(string &path) {
    std::vector<string> vec = {};
    std::ifstream istrm(path, std::ios::in);
    if (!istrm.is_open())
        throw Run_err("is_open err, file: " + path, TE_CUR_LOC);
    for (string line; std::getline(istrm, line);)
        vec.push_back(line);

    return vec;
}

string umap_str_to_str(std::unordered_map<string, string> &map) {
    string str;
    for (auto i : map)
        str += i.first + ":" + i.second + ", ";
    if (str.empty())
        str = "-- umap is empty --";
    return str;
}

int exec_proc(const char *path, char *argv[], char *envp[]) {
    pid_t pid = fork();
    if (pid < 0)
        throw Run_err("fork() failed", TE_ERR_LOC);

    if (pid == 0) {
        if (execve(path, argv, envp) == -1)
            throw Run_err("execve failed, path: " + string(path), TE_ERR_LOC);
    }

    return pid;
}

void term_proc(pid_t pid, int sig) {
    errno = 0;
    if (kill(pid, sig) < 0 && errno != ESRCH)
        throw Sys_err(errno,
                      "kill failed, pid: " + std::to_string(pid) +
                          " sig: " + std::to_string(sig),
                      TE_ERR_LOC);
}

void set_env(const char *name, const char *value) {
    if (setenv(name, value, 1) < 0)
        throw Sys_err(errno,
                      "set_env err, name: " + string(name) +
                          ", value: " + string(value),
                      TE_ERR_LOC);
}

std::vector<char *> str_vec_to_char_ptr_vec(std::vector<string> &vec) {
    std::vector<char *> char_vec;
    for (const auto &v : vec)
        char_vec.push_back((char *)v.data());
    char_vec.push_back(NULL);

    return char_vec;
}

} /* namespace Test */
} /* namespace Teave */