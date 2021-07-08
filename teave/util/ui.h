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
#ifndef TEAVE_UI_UTIL_H
#define TEAVE_UI_UTIL_H

namespace Teave {
namespace Util {

int u_to_p(float v, float scale);
float p_to_u(int v, float scale);
int perc_of_v(float perc, int v);

} // namespace Util
} // namespace Teave

#endif /* TEAVE_UI_UTIL_H */