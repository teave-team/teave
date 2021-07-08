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
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef EGL_MESA_platform_gbm
#error "EGL_MESA_platform_gbm not defined";
#endif

/* ensure unsigned char can hold 0-255, as needed for u_int8 */
static_assert(sizeof(unsigned char) >= 1,
              "invalid 'unsigned char' size, it must be >= 1");