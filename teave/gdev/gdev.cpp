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
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <teave/gl/gl_mgr.h>
#include <teave/gl/gl_util.h>
#include <teave/gdev/gdev_helpers.h>

#include <teave/include/stb_image/stb_image.h>

namespace Teave {

Egl_gbm_dev::Egl_gbm_dev(Err *const err) : err(err) {}

Egl_gbm_dev::~Egl_gbm_dev() {
    if (egl_dis) {
        eglMakeCurrent(egl_dis, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(egl_dis, egl_context);
            egl_context = EGL_NO_CONTEXT;
        }

        if (egl_sur != EGL_NO_SURFACE) {
            eglDestroySurface(egl_dis, egl_sur);
            egl_sur = EGL_NO_SURFACE;
        }
    }
    if (gbm_sur) {
        gbm_surface_destroy(gbm_sur);
        gbm_sur = nullptr;
    }

    if (egl_dis) {
        eglMakeCurrent(egl_dis, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglTerminate(egl_dis);
        egl_dis = nullptr;
    }

    if (gbm_dev) {
        gbm_device_destroy(gbm_dev);
        gbm_dev = nullptr;
    }
}

Egl_img::Egl_img(EGLImageKHR img, int width, int height,
                 Egl_fncs *const egl_fncs, EGLDisplay const egl_dis,
                 std::vector<int> &buf_fds)
    : img(img), width(width), height(height), egl_fncs(egl_fncs),
      egl_dis(egl_dis), buf_fds(buf_fds) {
    check_non_null_vars();
    if (buf_fds.empty())
        throw Arg_err("buf_fds is empty", TE_ERR_LOC);
}

Egl_img::Egl_img(EGLImageKHR img, int width, int height,
                 Egl_fncs *const egl_fncs, EGLDisplay const egl_dis)
    : img(img), width(width), height(height), egl_fncs(egl_fncs),
      egl_dis(egl_dis) {
    check_non_null_vars();
}

Egl_img::~Egl_img() {
    if (img)
        egl_fncs->egl_destroy_image(egl_dis, img);

    for (auto &fd : buf_fds)
        close(fd);
}

void Egl_img::check_non_null_vars() {
    if (!img)
        throw Arg_err("img is null", TE_ERR_LOC);
    if (!egl_fncs)
        throw Arg_err("egl_fncs is null", TE_ERR_LOC);
    if (!egl_dis)
        throw Arg_err("egl_dis is null", TE_ERR_LOC);
}

/* was not able to ctor init list the creation of bufs array with Gbus(err,dev)
 * in con so removed the err and dev from ctor args
 */
Gbuf::Gbuf() {}

Gbuf::~Gbuf() { suspend(); }

void Gbuf::suspend() {
    drmModeRmFB(dev->dev_f->fd, drm_id);
    drm_id = 0;

    if (bo) {
        gbm_surface_release_buffer(dev->eg_dev.gbm_sur, bo);
        bo = nullptr;
    }
}

Gcon::Gcon(Err *const err, Gdev *const dev) : err(err), dev(dev) {
    /* was not able to ctor init list the creation of bufs array with
     * Gbus(err,dev) */
    for (auto &b : bufs) {
        b.err = err;
        b.dev = dev;
    }
}

Gcon::~Gcon() { drmModeFreeCrtc(saved_crtc); }

Gdev::Gdev(Err *const err, Dev_f::uptr dev_f_, Settings *settings)
    : err(err), dev_f(std::move(dev_f_)), settings(settings), eg_dev(err) {}

Gdev::~Gdev() {
    ready = false;
    for (Gcon::uptr &con : cons)
        restore_saved_crtc(this, con.get());
    /* explicitly clearing to make sure we don't have to worry about
     * order of vars in Gdev. Gcons depend on Egl_gbm_dev, and Gcons
     * must be destructed before Egl_gbm_dev */
    cons.clear();
}

} // namespace Teave