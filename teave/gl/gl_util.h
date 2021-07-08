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
#ifndef TEAVE_GL_UTIL_H
#define TEAVE_GL_UTIL_H

#include <teave/err/err.h>
#include <EGL/egl.h>

namespace Teave {
namespace Gl_util {

/* note for EGL (not GL) */
bool pro_egl_errs(Err *const err, std::string gdev_path,
                  std::string subject_fnc, EGLBoolean ret,
                  std::string caller_fnc, std::string info = "");

/* note for GL (not EGL) */
bool pro_gl_errs(Err *const err, std::string gdev_path, std::string subject_fnc,
                 std::string caller_fnc, std::string info = "");

std::string get_gl_errs();

} // namespace Gl_util
} // namespace Teave

#endif /* TEAVE_GL_UTIL_H */
