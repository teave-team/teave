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
#include <teave/util/file.h>
#include <teave/err/err.h>
#include <dirent.h>
#include <cstring>
#include <experimental/filesystem>
#include <unistd.h>
#include <fcntl.h>

namespace fs = std::experimental::filesystem;

namespace Teave {
namespace Fs {

using vec_str = std::vector<std::string>;
using openmode = std::ios_base::openmode;

static void open_file_write(std::ofstream &fs, const string &file_path,
                            const openmode &mode) {
    if (file_path.empty())
        throw Arg_err("file_path is empty", TE_ERR_LOC);

    fs.open(file_path, mode);
    if (!fs.is_open()) {
        /* if file doesn't exist create dirs in file path if needed, and try
         * opening/creating again */
        std::vector<string> dirs;
        string tmp = file_path;
        char *token = std::strtok(tmp.data(), "/");
        while (token != NULL) {
            string s(token);
            dirs.emplace_back(s);
            token = std::strtok(NULL, "/");
        }
        dirs.pop_back();
        string dirs_str;
        for (auto &d : dirs)
            dirs_str += "/" + d;

        if (file_path.front() !=
            '/') /* if not absolute path, remove the first added '/' */
            dirs_str = dirs_str.substr(1);

        std::error_code ec;
        if (!fs::create_directories(dirs_str, ec)) {
            if (ec.value() > 0)
                throw Sys_err(ec.value(),
                              "create_directories failed, path: " + dirs_str,
                              TE_ERR_LOC);
        }
        fs.open(file_path, mode);
        if (!fs.is_open())
            throw Sys_err(errno, "fs.open failed for file_path:" + file_path,
                          TE_ERR_LOC);
    }
}

void write_to_file(const string &file_path, const string &content,
                   const openmode &mode) {
    std::ofstream fs;
    open_file_write(fs, file_path, mode);

    fs << content << std::endl;
    fs.close();
}

void write_to_file(const string &file_path, const vec_str &content,
                   const openmode &mode) {

    std::ofstream fs;
    open_file_write(fs, file_path, mode);

    for (const auto &line : content)
        fs << line << std::endl;

    fs.close();
}

string read_file(const string &file_path) {
    std::ifstream fs(file_path);
    if (!fs.is_open())
        throw Run_err("could not open file: " + file_path, TE_ERR_LOC);
    std::stringstream str_stream;
    str_stream << fs.rdbuf();
    fs.close();

    return str_stream.str();
}

void remove_file(string &path) {
    std::error_code ec;
    if (!fs::remove(path.c_str(), ec)) {
        if (ec.value() > 0)
            throw Sys_err(ec.value(), "remove failed, file: " + path,
                          TE_ERR_LOC);
    }
}

void remove_file(string &&path) {
    std::error_code ec;
    if (!fs::remove(path.c_str(), ec)) {
        if (ec.value() > 0)
            throw Sys_err(ec.value(), "remove failed, file: " + path,
                          TE_ERR_LOC);
    }
}

void change_file_ownership(string path, int uid, int gid) {
    if (chown(path.c_str(), uid, gid) < 0)
        throw Sys_err(errno,
                      "chown err, path: " + path +
                          " to user_id: " + std::to_string(uid) +
                          " group_id: " + std::to_string(gid),
                      TE_ERR_LOC);
}

int open_path(const char *path, int flags) { return open(path, flags); }

void list_dir(string &&dir_path, std::vector<string> &list, int &err_n) {
    DIR *dir = opendir(dir_path.c_str());
    if (!dir)
        throw Sys_err(errno, "opendir err, dir_path: " + dir_path, TE_ERR_LOC);

    struct dirent *dp;

    errno = 0;

    while ((dp = readdir(dir)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
        } else {
            list.emplace_back(dp->d_name);
        }
    }

    err_n = errno;

    closedir(dir);
}

} // namespace Fs
} // namespace Teave
