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
#ifndef TEAVE_FILE_UTIL_H
#define TEAVE_FILE_UTIL_H

#include <string>
#include <vector>
#include <fstream>
#include <vector>

namespace Teave {
namespace Fs {

using std::string;

void write_to_file(const string &file_path, const string &content,
                   const std::ios_base::openmode &mode = std::ios_base::app);

void write_to_file(const string &file_path, const std::vector<string> &content,
                   const std::ios_base::openmode &mode = std::ios_base::app);

string read_file(const string &file_path);

void remove_file(string &&file_path);

void remove_file(string &file_path);

void change_file_ownership(string path, int uid, int gid);

int open_path(const char *path, int flags);

void list_dir(string &&dir_path, std::vector<string> &list, int &err_n);

} // namespace Fs
} // namespace Teave

#endif /* TEAVE_FILE_UTIL_H */
