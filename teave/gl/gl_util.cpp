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
#include <teave/gl/gl_util.h>
#include <teave/util/util.h>
#include <GLES3/gl32.h>

using std::string;

namespace Teave {
namespace Gl_util {

bool pro_egl_errs(Err *const err, string gdev_path, string subject_fnc,
                  EGLBoolean ret, string caller_fnc, string info) {
    if (ret == EGL_TRUE)
        return true;

    EGLint g_err = eglGetError();

    if (info != "")
        info = string(" ") + info;

    err->log(subject_fnc + string(" returned EGL_FALSE and generated err: ") +
                 Util::int_to_hex_str(g_err) + info + string(" ") +
                 "gdev_path: " + gdev_path + " caller_fnc: " + caller_fnc,
             TE_ERR_LOC);

    return false;
}

bool pro_gl_errs(Err *const err, string gdev_path, string subject_fnc,
                 string caller_fnc, string info) {
    bool ret = true;
    GLenum g_err;
    if (info != "")
        info = string(" ") + info;

    do {
        g_err = glGetError();
        if (g_err != GL_NO_ERROR) {
            ret = false;
            string str = subject_fnc + string(" err: ") +
                         Util::int_to_hex_str(g_err) + info;

            str += string(" ") + gdev_path;
            err->log(str + " caller_fnc: " + caller_fnc, TE_ERR_LOC);
        }

    } while (g_err != GL_NO_ERROR);

    return ret;
}

string get_gl_errs() {
    string str;
    GLenum g_err;
    do {
        g_err = glGetError();
        if (g_err != GL_NO_ERROR) {
            str += string(" err: ") + Util::int_to_hex_str(g_err);
        }

    } while (g_err != GL_NO_ERROR);

    return str;
}

} /* namespace Gl_util */
} /* namespace Teave */
