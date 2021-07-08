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
#ifndef TEAVE_SV_DEV_H
#define TEAVE_SV_DEV_H

#include <teave/err/err.h>
#include <unordered_map>

namespace Te = Teave;

namespace Teave_sv {
using std::string;

struct Dev {
    using uptr = std::unique_ptr<Dev>;
    enum class Type { gdev, idev, dev, none };
    Dev(Te::Err *err, string &path, Type type, int open_flags);
    ~Dev();
    Te::Err *err = nullptr;
    int fd = -1;
    string path;
    Type type = Type::none;
};

struct Gdev : public Dev {
    using uptr = std::unique_ptr<Gdev>;
    Gdev(Te::Err *err, string &path, int flags);
    ~Gdev();
    int max_width = 0;
    int min_width = 0;
    int max_height = 0;
    int min_height = 0;
};

} // namespace Teave_sv
#endif /* TEAVE_SV_DEV_H */
