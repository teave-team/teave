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
#include <teave/util/uuid_gen.h>
#include <teave/include/boost/uuid/uuid_io.hpp>

namespace Teave {

Uuid Uuid_gen::gen_uuid() { return gen(); }
std::string Uuid_gen::to_str(Uuid &uid) { return boost::uuids::to_string(uid); }

} // namespace Teave