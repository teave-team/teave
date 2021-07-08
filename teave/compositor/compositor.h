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

#ifndef TEAVE_COMPOSITOR_H
#define TEAVE_COMPOSITOR_H

#include <teave/gdev/gdev.h>

namespace Teave {

class Compositor {
public:
    virtual ~Compositor();
    virtual void process_vblank(Gcon *con) = 0;
};

} // namespace Teave

#endif /* TEAVE_COMPOSITOR_H */
