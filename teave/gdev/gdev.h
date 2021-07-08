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
#ifndef TEAVE_GDEV_H
#define TEAVE_GDEV_H

#include <teave/err/err.h>
#include <teave/gl/egl_fncs.h>
#include <teave/device/udev.h>
#include <teave/common.h>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <unordered_map>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <functional>
#include <gbm.h>

namespace Teave {
using std::string;

struct Egl_gbm_dev {
    Egl_gbm_dev(Err *const err);
    ~Egl_gbm_dev();

    Err *const err = nullptr;
    int gbm_fd = 0;
    struct gbm_device *gbm_dev = nullptr;
    gbm_surface *gbm_sur = nullptr;

    EGLDisplay egl_dis = nullptr;
    EGLConfig egl_config = nullptr;
    EGLContext egl_context = EGL_NO_CONTEXT;
    EGLSurface egl_sur = EGL_NO_SURFACE;

    /*
     * egl/gbm surface width and height
     * width: the width of the con with the largest width
     * height: the height of the con with the largest height
     */
    uint32_t sur_width = 0;
    uint32_t sur_height = 0;
};

struct Egl_img {
    using uptr = std::unique_ptr<Egl_img>;
    Egl_img(EGLImageKHR img, int width, int height, Egl_fncs *const egl_fncs,
            EGLDisplay const egl_dis);

    Egl_img(EGLImageKHR img, int width, int height, Egl_fncs *const egl_fncs,
            EGLDisplay const egl_dis, std::vector<int> &buf_fds);

    ~Egl_img();

    EGLImageKHR img = nullptr;
    int width = 0;
    int height = 0;
    Egl_fncs *const egl_fncs = nullptr;
    EGLDisplay const egl_dis = nullptr;
    /* fds that were used to import EGLImage
    using EGL_EXT_image_dma_buf_import, Egl_img will close them in dtor */
    std::vector<int> buf_fds;

private:
    void check_non_null_vars();
};

struct Gdev;
struct Gcon;

struct Gbuf {
    Gbuf();
    ~Gbuf();
    void suspend();
    Err *err = nullptr;
    Gdev *dev = nullptr; // needed for removing drm_id (see suspend)

    struct gbm_bo *bo = nullptr;
    uint32_t drm_id = 0; // provided by drmModeAddFB

    int width = 0;
    int height = 0;
};

class Compositor;
/* Graphics Device Connector */
struct Gcon {
    using uptr = std::unique_ptr<Gcon>;

    Gcon(Err *const err, Gdev *const dev);
    ~Gcon();

    Err *const err = nullptr;
    Gdev *const dev = nullptr; // reference to the gdev

    Compositor *comp = nullptr; // reference to compositor, used in drm.cpp::te_vblank_handler

    int id = 0; // used for identifying the Gcon, not to be confused by gdev_con_id.
    string name;
    uint32_t gdev_con_id =
        0; // used only for drm and graphics device operations.

    /* render */
    /* flag indicating whether we should render this con
     * ie has there been updates that need to be rendered
     */
    bool render = false;
    /* render callback */
    std::function<void(void *)> render_fnc = nullptr;

    /* con number (starting from 1), 1st monitor, 2nd, etc. We use for
    scheduling vblanks see Gdev_mgr::schedule_vblank
    */

    bool vbl_vars_started =
        false; // flag to note that we have started loading vbl variables
    unsigned int vbl_seq = 0;         // vblank squence number
    unsigned int vbl_seq_ts_sec = 0;  // timestamp (sec) of seq number
    unsigned int vbl_seq_ts_usec = 0; // timestamp (sec) of seq number

    unsigned int vbl_prev_seq = 0; // vblank previous squence number
    unsigned int vbl_prev_seq_ts_sec =
        0; // timestamp (sec) of previous seq number
    unsigned int vbl_prev_seq_ts_usec =
        0; // timestamp (usec) of previous seq number
    unsigned int vbl_prd =
        0; // vblank period in  usec/micro (time separation between each vblank)

    /* double buffering */
    std::array<Gbuf, 2> bufs;
    int front_buf = 0; // front buffer index

    drmModeCrtc *saved_crtc = nullptr;
    uint32_t crtc_id = 0;
    drmModeModeInfo mode;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t x = 0;
    uint32_t y = 0;

    /* compositor world offsets */
    uint32_t x_of = 0;
    uint32_t y_of = 0;

    void *data = nullptr;
};

class Gl_mgr;

class Settings;
/* Graphics Device/Card */
struct Gdev {
    using uptr = std::unique_ptr<Gdev>;

    Gdev(Err *const err, Dev_f::uptr dev_f, Settings *settings);
    ~Gdev();
    void gl_setup(Gl_mgr *const gl_mgr);

    int id = 0;
    Err *const err = nullptr;
    Dev_f::uptr dev_f = nullptr;
    Settings *settings = nullptr;
    bool resume_req = false;
    bool ready = false;

    std::vector<Gcon::uptr> cons; /* connectors ie. each attached monitor */
    /* flag to save calls for multi connectors/monitors if we only have one
     * monitor */
    bool multi_con = false;
    Egl_gbm_dev eg_dev;

    uint32_t min_width = 0;
    uint32_t max_width = 0;
    uint32_t min_height = 0;
    uint32_t max_height = 0;

    void *data = nullptr;
};

} // namespace Teave

#endif /* TEAVE_GDEV_H */