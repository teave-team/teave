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
#ifndef TEAVE_GL_MGR_H
#define TEAVE_GL_MGR_H

#include <teave/err/err.h>
#include <teave/gdev/gdev_mgr.h>
#include <teave/setting/settings.h>
#include <teave/gl/egl_fncs.h>
#include <teave/event/event.h>

void gl_msg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *user_data);

namespace Teave {

class Gl_mgr {
public:
    using string = std::string;
    using uptr = std::unique_ptr<Gl_mgr>;

    /* these are public because we use them in gl_msg_callback */
    Err *const err;

    Gl_mgr(Err *const err, Settings *const settings, Gdev_mgr *const gdev_mgr);
    void load_gdev_gl(Gdev *dev);
    void make_current(Gdev *dev);

    /* these must be call after load_gdev_gl and make_current */
    void drm_add_fb(int fd, gbm_bo *bo, uint32_t &buf_id);
    void release_buf(Gcon *con, Gbuf &buf);
    void set_crtc(Gcon *con, uint32_t buf_id);

    /**
     * checking gl extesions requires gl context, this is public so that we
     * could call from compositor
     */
    void check_gl_extensions(Gdev *dev);
    void check_state(Gdev *dev);

private:
    void setup();
    /**
     *
     * @param dev: if NULL it checks EGL app extensions
     *  if not NULL it checks EGL display extensions
     */
    void check_egl_extensions(Gdev *dev);

    void init_gbm(Gdev *dev);
    void init_egl(Gdev *dev);
    bool load_egl_configs(Gdev *dev);
    void load_gdev_egl_sur(Gdev *dev);

    /* process egl api return values */
    /*
     * helper for processing egl returns and error codes from eglGetError()
     *
     * returns 0, if the egl function (subject_fnc) returned success
     * returns -1, if egl generated error codes
     *
     * caller_fnc: the function that we had call the egl function in
     * subject_fnc: the egl function that we called
     * info: any additional info we want to log in case of an error
     */
    bool pro_egl_errs(Gdev *dev, string subject_fnc, string caller_fnc,
                      string info = "");

    /* same as above with EGLBoolean ret */
    bool pro_egl_errs(Gdev *dev, string subject_fnc, EGLBoolean ret,
                      string caller_fnc, string info = "");

    Settings *const settings;
    Gdev_mgr *const gdev_mgr;
    Egl_fncs egl_fncs;
};

} // namespace Teave

#endif /* TEAVE_GL_MGR_H */
