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
#ifndef TEAVE_UTILITY_H
#define TEAVE_UTILITY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

namespace Teave {
namespace Util {

using std::string;

template <class T> int class_enum_to_int(T t) {
    return static_cast<typename std::underlying_type<T>::type>(t);
}

string vec_str_to_str(const std::vector<string> &vec, string separator = ",");
string vec_int_to_str(const std::vector<int> &vec, string separator = ",");
string vec_char_to_str(const std::vector<char> &vec);
string umap_str_str_to_str(const std::unordered_map<string, string> &map,
                           string separator = ",");
std::vector<string> split_str(string &subject, string separator,
                              bool trim = true);
string int_to_hex_str(int num);
string ascii_str_to_lower(const string &str);
/* valid/invalid str, value */
std::tuple<bool, bool> str_to_bool(const string &str);

size_t get_size(const char *str);

unsigned long timestamp_micro();

} // namespace Util
} // namespace Teave

#endif /* TEAVE_UTILITY_H */
