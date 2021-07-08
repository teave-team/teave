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
#include "teave/util/util.h"
#include "teave/err/err.h"
#include <sys/time.h>
#include "teave/include/boost/system/error_code.hpp"
#include "teave/include/boost/algorithm/string.hpp"
#include <sstream>

namespace bsy = boost::system;

namespace Teave {
namespace Util {

using std::string;

std::string vec_str_to_str(const std::vector<std::string> &vec,
                           std::string separator) {
    string ret;
    for (const auto &item : vec)
        ret += item + separator;

    if (!ret.empty())
        ret.pop_back();

    return ret;
}

std::string vec_int_to_str(const std::vector<int> &vec, string separator) {
    string ret;
    for (const auto &item : vec)
        ret += std::to_string(item) + separator;
    if (!ret.empty())
        ret.pop_back();

    return ret;
}

string vec_char_to_str(const std::vector<char> &vec) {
    string ret;
    for (char c : vec)
        ret.append(1, c);
    if (!ret.empty())
        ret.pop_back();

    return ret;
}

std::vector<string> split_str(string &subject, string separator, bool trim) {
    std::vector<string> vec;
    boost::split(vec, subject, boost::is_any_of(separator),
                 boost::token_compress_on);
    for (auto &v : vec)
        boost::trim(v);
    return vec;
}

string umap_str_str_to_str(const std::unordered_map<string, string> &map,
                           string separator) {
    string ret;
    for (auto &it : map)
        ret += it.first + ":" + it.second + separator;
    if (!ret.empty())
        ret.pop_back();
    return ret;
}

string int_to_hex_str(int num) {
    std::stringstream stream;
    stream << "0x" << std::hex << num;
    std::string result(stream.str());
    return result;
}

string ascii_str_to_lower(const string &str) {
    return boost::algorithm::to_lower_copy(str);
}

std::tuple<bool, bool> str_to_bool(const string &str) {
    string ls = ascii_str_to_lower(str);
    if (ls == "true")
        return {true, true};
    if (ls == "false")
        return {true, false};
    return {false, false}; /* invalid value */
}

size_t get_size(const char *str) { return strlen(str); }

unsigned long timestamp_micro() {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) < 0)
        throw Sys_err(errno, "gettimeofday err", TE_ERR_LOC);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

} // namespace Util
} // namespace Teave
