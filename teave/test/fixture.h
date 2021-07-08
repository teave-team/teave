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
#include <string>
#include <teave/debug/debug.h>
#include <teave/test/util.h>

namespace Teave {
namespace Test {
using std::string;

class Fixture {
public:
    Fixture();

protected:
    Err err;
};

} // namespace Test
} // namespace Teave