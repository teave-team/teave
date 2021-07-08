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
#ifndef TEAVE_COMMON_H
#define TEAVE_COMMON_H

#include <string>

/* u_int8 is used to hold image data, must be able to hold 0-255 */
typedef unsigned char u_int8;

/**
 *  preprocessors set in CMAKE
 *  we need to "Stringize" the string constants set in CMake otherwise we'll
 *  have issues with the expansion of the string constant.
 *  see https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html#Stringizing
 */
#define TE_MAKE_STR_1(str) TE_MAKE_STR_2(str)
#define TE_MAKE_STR_2(str) #str

#define TE_APP_NAME TE_MAKE_STR_1(TE_APP_NAME_)

namespace Teave {
enum Proc_type { PROC_TEAVE_SV, PROC_APP };

} /* namespace Teave */

#endif /* TEAVE_COMMON_H */
