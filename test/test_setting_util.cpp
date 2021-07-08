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
#include <teave/include/catch2/catch.hpp>
#include <teave/setting/setting_util.h>
#include <teave/test/util.h>

namespace Te = Teave;
namespace Tsu = Teave::Setting_util;
using std::string;
using umap_str = std::unordered_map<string,string>;
using vec_str = std::vector<string>;
TEST_CASE( "setting_util parse_argv", "[setting][setting_util][parse_argv]" ) {
    /* key,value */
    umap_str k_v;
    SECTION("--key=vals are parsed"){
        vec_str args = {"./app", "--dir=/path/path"};
        umap_str expect = {
            {"dir","/path/path"}
        };
        std::vector<char*> argv_ = Te::Test::str_vec_to_char_ptr_vec(args);
        Tsu::parse_argv(k_v, argv_.size(), argv_.data());
        INFO(Te::Test::umap_str_to_str(k_v));
        REQUIRE(expect == k_v);
    }

    SECTION("key=vals are parsed (input err)"){
        /* this would be an input err in the app */
        vec_str args = {"./app", "dir=/path/path"};
        umap_str expect = {
            {"r","/path/path"}
        };
        std::vector<char*> argv_ = Te::Test::str_vec_to_char_ptr_vec(args);
        Tsu::parse_argv(k_v, argv_.size(), argv_.data());
        INFO(Te::Test::umap_str_to_str(k_v));
        REQUIRE(expect == k_v);
    }

    SECTION("key are parsed (input err)"){
        /* this would be an input err in the app */
        vec_str args = {"./app", "dir"};
        umap_str expect = {
            {"r",""}
        };
        std::vector<char*> argv_ = Te::Test::str_vec_to_char_ptr_vec(args);
        Tsu::parse_argv(k_v, argv_.size(), argv_.data());
        INFO(Te::Test::umap_str_to_str(k_v));
        REQUIRE(expect == k_v);
    }

    SECTION("--key= are parsed"){
        vec_str args = {"./app", "--dir"};
        umap_str expect = {
            {"dir",""}
        };
        std::vector<char*> argv_ = Te::Test::str_vec_to_char_ptr_vec(args);
        Tsu::parse_argv(k_v, argv_.size(), argv_.data());
        INFO(Te::Test::umap_str_to_str(k_v));
        REQUIRE(expect == k_v);
    }

}
