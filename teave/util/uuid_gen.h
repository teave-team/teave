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
#ifndef TEAVE_UUID_GEN_H
#define TEAVE_UUID_GEN_H

#ifndef BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_ALLOW_DEPRECATED_HEADERS
#endif

#include <teave/include/boost/uuid/uuid.hpp>
#include <teave/include/boost/uuid/uuid_generators.hpp>

namespace Teave {

using Uuid = boost::uuids::uuid;

class Uuid_gen {
public:
    Uuid gen_uuid();
    std::string to_str(Uuid &uid);

protected:
    boost::uuids::random_generator gen;
};

} // namespace Teave

#endif /* TEAVE_UUID_GEN_H */