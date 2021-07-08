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
#include <teave/gl/gl_mgr.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <teave/gdev/gdev_helpers.h>
#include <algorithm>
#include <teave/util/util.h>
#include <teave/gl/gl_util.h>
#include <teave/include/stb_image/stb_image.h>

using std::string;
using namespace Teave::Util;
using namespace Teave::Gl_util;

void gl_msg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *user_data) {
    Teave::Gl_mgr *gl_mgr = (Teave::Gl_mgr *)user_data;
    if (gl_mgr)
        throw Teave::Arg_err("user_data is null", TE_ERR_LOC);

    if (type == GL_DEBUG_TYPE_ERROR)
        gl_mgr->err->log("GL ERROR message: " + string(message), TE_ERR_LOC);
}

namespace Teave {

Gl_mgr::Gl_mgr(Err *const err, Settings *const settings,
               Gdev_mgr *const gdev_mgr)
    : err(err), settings(settings), gdev_mgr(gdev_mgr) {
    if (!err)
        throw Arg_err("err is null", TE_ERR_LOC);
    if (!settings)
        throw Arg_err("settings is null", TE_ERR_LOC);
    if (!gdev_mgr)
        throw Arg_err("gdev_mgr is null", TE_ERR_LOC);

    check_egl_extensions(nullptr); // check EGL app extensions
    setup();
    for (auto &dev : gdev_mgr->devs)
        check_egl_extensions(dev.get()); // check EGL display extensions
}

void Gl_mgr::setup() {
    for (auto &dev : gdev_mgr->devs) {
        init_gbm(dev.get());
        init_egl(dev.get());
    }
}

void Gl_mgr::init_gbm(Gdev *dev) {
    dev->eg_dev.gbm_dev = gbm_create_device(dev->dev_f->fd);
    if (!dev->eg_dev.gbm_dev)
        throw Run_err("gbm_create_device err, " + gdev_info(dev), TE_ERR_LOC);

    dev->eg_dev.gbm_fd = gbm_device_get_fd(dev->eg_dev.gbm_dev);
}

void Gl_mgr::init_egl(Gdev *dev) {
    if (!dev->eg_dev.gbm_dev)
        throw Arg_err("dev->eg_dev.gbm_dev is null", TE_ERR_LOC);

    dev->eg_dev.egl_dis = eglGetPlatformDisplay(EGL_PLATFORM_GBM_MESA,
                                                dev->eg_dev.gbm_dev, nullptr);
    //    dev->eg_dev.egl_dis = eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_MESA,
    //    dev->eg_dev.gbm_dev, NULL);

    if (dev->eg_dev.egl_dis == EGL_NO_DISPLAY)
        throw Run_err("eglGetPlatformDisplay returned EGL_NO_DISPLAY, " +
                          gdev_info(dev),
                      TE_ERR_LOC);

    EGLint major, minor;
    EGLBoolean ret = eglInitialize(dev->eg_dev.egl_dis, &major, &minor);
    if (!this->pro_egl_errs(dev, "eglInitialize", ret, TE_ERR_LOC)) {
        eglTerminate(dev->eg_dev.egl_dis);
        throw Run_err("eglInitialize err", TE_ERR_LOC);
    }

    if (err->get_logger()->get_log_lvl() == Log_lvl::info)
        err->log("Egl version major: " + std::to_string(major) +
                     ", minor: " + std::to_string(minor),
                 TE_ERR_LOC);

    ret = eglBindAPI(EGL_OPENGL_ES_API);
    if (!this->pro_egl_errs(dev, "eglBindAPI", ret, TE_ERR_LOC)) {
        eglTerminate(dev->eg_dev.egl_dis);
        throw Run_err("eglBindAPI err", TE_ERR_LOC);
    }

    if (!this->load_egl_configs(dev)) {
        eglTerminate(dev->eg_dev.egl_dis);
        throw Run_err("load_egl_configs was not able to load configs",
                      TE_ERR_LOC);
    }
}

/*
 * on success dev->eg_dev.egl_config is loaded and true returned
 * on failure dev->eg_dev.egl_config is set to NULL and false is returned
 */
bool Gl_mgr::load_egl_configs(Gdev *dev) {
    EGLint const config_attrs[] = {EGL_RED_SIZE,
                                   8,
                                   EGL_GREEN_SIZE,
                                   8,
                                   EGL_BLUE_SIZE,
                                   8,
                                   EGL_DEPTH_SIZE,
                                   24,
                                   EGL_CONFORMANT,
                                   EGL_OPENGL_ES3_BIT_KHR,
                                   EGL_RENDERABLE_TYPE,
                                   EGL_OPENGL_ES3_BIT_KHR,
                                   EGL_SURFACE_TYPE,
                                   EGL_WINDOW_BIT,
                                   EGL_NATIVE_RENDERABLE,
                                   EGL_TRUE,
                                   EGL_NONE};

    EGLint num_configs = 0;
    EGLBoolean ret = eglGetConfigs(dev->eg_dev.egl_dis, NULL, 0, &num_configs);
    if (!this->pro_egl_errs(dev, "eglGetConfigs", ret, TE_ERR_LOC))
        return false;

    EGLConfig *configs = nullptr;
    configs = (EGLConfig *)malloc(num_configs * sizeof(EGLConfig));
    ret = eglChooseConfig(dev->eg_dev.egl_dis, config_attrs, configs,
                          num_configs, &num_configs);
    if (!pro_egl_errs(dev, "eglChooseConfig", ret, TE_ERR_LOC)) {
        if (configs)
            free(configs);
        return false;
    }

    if (num_configs < 1) {
        err->log(string("eglChooseConfig didn't find any configs ") +
                     gdev_info(dev),
                 TE_ERR_LOC);
        if (configs)
            free(configs);
        return false;
    }

    EGLint attr_value;
    dev->eg_dev.egl_config = NULL;
    /* find the config with EGL_NATIVE_VISUAL_ID == GBM_FORMAT_XRGB8888 */
    for (int i = 0; i < num_configs; ++i) {
        attr_value = -1;
        ret = eglGetConfigAttrib(dev->eg_dev.egl_dis, configs[i],
                                 EGL_NATIVE_VISUAL_ID, &attr_value);
        if (!pro_egl_errs(dev, "eglGetConfigAttrib", ret, TE_ERR_LOC)) {
            if (configs)
                free(configs);
            return false;
        }

        if (attr_value == GBM_FORMAT_XRGB8888) {
            dev->eg_dev.egl_config = configs[i];
            break;
        }
    }

    if (configs)
        free(configs);

    if (dev->eg_dev.egl_config == NULL)

        return false;

    return true;
}

void Gl_mgr::make_current(Gdev *dev) {
    if (!dev)
        throw Arg_err("dev is null, dev: " + gdev_info(dev), TE_ERR_LOC);
    if (!dev->eg_dev.egl_dis)
        throw Run_err("egl_dis is null, dev: " + gdev_info(dev), TE_ERR_LOC);
    if (!dev->eg_dev.egl_sur)
        throw Run_err("egl_sur is null, dev: " + gdev_info(dev), TE_ERR_LOC);
    if (!dev->eg_dev.egl_context)
        throw Run_err("egl_context is null, dev: " + gdev_info(dev),
                      TE_ERR_LOC);

    EGLBoolean ret =
        eglMakeCurrent(dev->eg_dev.egl_dis, dev->eg_dev.egl_sur,
                       dev->eg_dev.egl_sur, dev->eg_dev.egl_context);
    if (!pro_egl_errs(dev, "eglMakeCurrent", ret, TE_ERR_LOC))
        throw Run_err("eglMakeCurrent failed, dev: " + gdev_info(dev),
                      TE_ERR_LOC);
}

void Gl_mgr::load_gdev_gl(Gdev *dev) {
    if (!dev)
        throw Arg_err("dev is null", TE_ERR_LOC);

    /* gbm surface */
    dev->eg_dev.gbm_sur = gbm_surface_create(
        dev->eg_dev.gbm_dev, dev->eg_dev.sur_width, dev->eg_dev.sur_height,
        GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!dev->eg_dev.gbm_sur)
        throw Run_err("could not create gbm surface, width:" +
                          std::to_string(dev->eg_dev.sur_width) +
                          " height:" + std::to_string(dev->eg_dev.sur_height) +
                          gdev_info(dev),
                      TE_ERR_LOC);

    /* egl surface */
    load_gdev_egl_sur(dev);
}

void Gl_mgr::load_gdev_egl_sur(Gdev *dev) {
    /* EGL context attributes */
    std::vector<EGLint> cntxt_att = {EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
                                     EGL_CONTEXT_MINOR_VERSION_KHR, 2};
    if (err->get_logger()->get_log_lvl() == Log_lvl::info) {
        cntxt_att.insert(cntxt_att.end(), {EGL_CONTEXT_FLAGS_KHR,
                                           EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR});
    }
    cntxt_att.push_back(EGL_NONE);
    dev->eg_dev.egl_context =
        eglCreateContext(dev->eg_dev.egl_dis, dev->eg_dev.egl_config,
                         EGL_NO_CONTEXT, cntxt_att.data());

    if (dev->eg_dev.egl_context == EGL_NO_CONTEXT) {
        pro_egl_errs(dev, "eglCreateContext", TE_ERR_LOC);
        throw Run_err("eglCreateContext returned EGL_NO_CONTEXT, " +
                          gdev_info(dev),
                      TE_ERR_LOC);
    }

    EGLAttrib const sur_attrs[] = {EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
                                   EGL_NONE};

    dev->eg_dev.egl_sur = eglCreatePlatformWindowSurface(
        dev->eg_dev.egl_dis, dev->eg_dev.egl_config, dev->eg_dev.gbm_sur,
        sur_attrs);

    //    dev->eg_dev.egl_sur =
    //    eglCreatePlatformWindowSurfaceEXT(dev->eg_dev.dev->eg_dev.egl_dis,
    //            dev->eg_dev.dev->eg_dev.egl_config, dev->eg_dev.gbm_sur,
    //            sur_attrs);

    if (dev->eg_dev.egl_sur == EGL_NO_SURFACE) {
        pro_egl_errs(dev, "eglCreateWindowSurface", TE_ERR_LOC);
        throw Run_err(
            "eglCreatePlatformWindowSurface returned EGL_NO_SURFACE, " +
                gdev_info(dev),
            TE_ERR_LOC);
    }

    if (err->get_logger()->get_log_lvl() == Log_lvl::info) {
        glEnable(GL_DEBUG_OUTPUT);
        if (!pro_gl_errs(err, dev->dev_f->path, "glEnable GL_DEBUG_OUTPUT",
                         TE_ERR_LOC))
            throw Run_err("glEnable(GL_DEBUG_OUTPUT) failed", TE_ERR_LOC);

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        if (!pro_gl_errs(err, dev->dev_f->path,
                         "glEnable GL_DEBUG_OUTPUT_SYNCHRONOUS", TE_ERR_LOC))
            throw Run_err("glEnable GL_DEBUG_OUTPUT_SYNCHRONOUS failed",
                          TE_ERR_LOC);

        glDebugMessageCallback(gl_msg_callback, this);
    }
}

void Gl_mgr::drm_add_fb(int fd, gbm_bo *bo, uint32_t &buf_id) {
    int bo_width = gbm_bo_get_width(bo);
    int bo_height = gbm_bo_get_height(bo);

    if (bo_width < 1 || bo_height < 1)
        Run_err(string("invalid bo_width or bo_height,") +
                    string(" bo_width: ") + std::to_string(bo_width) +
                    string(" bo_height: ") + std::to_string(bo_height),
                TE_ERR_LOC);

    int ret =
        drmModeAddFB(fd, bo_width, bo_height, 24, 32, gbm_bo_get_stride(bo),
                     gbm_bo_get_handle(bo).u32, &buf_id);

    if (ret < 0)
        Sys_err(errno,
                "drmModeAddFB failed bo_width: " + std::to_string(bo_width) +
                    " bo_height: " + std::to_string(bo_height),
                TE_ERR_LOC);
}

void Gl_mgr::release_buf(Gcon *con, Gbuf &buf) {
    gbm_surface_release_buffer(con->dev->eg_dev.gbm_sur, buf.bo);
    buf.bo = nullptr;
    drmModeRmFB(con->dev->dev_f->fd, buf.drm_id);
}

void Gl_mgr::set_crtc(Gcon *con, uint32_t buf_id) {
    int ret = drmModeSetCrtc(con->dev->dev_f->fd, con->crtc_id, buf_id, con->x,
                             con->y, &con->gdev_con_id, 1, &con->mode);
    if (ret < 0)
        Sys_err(errno,
                "drmModeSetCrtc err, dev path: " + con->dev->dev_f->path +
                    " con->width: " + std::to_string(con->width) +
                    " con->height: " + std::to_string(con->height) +
                    " buf_id: " + std::to_string(buf_id),
                TE_ERR_LOC);
}

bool Gl_mgr::pro_egl_errs(Gdev *dev, string subject_fnc, EGLBoolean ret,
                          string caller_fnc, string info) {
    if (ret == EGL_TRUE)
        return true;

    EGLint g_err = eglGetError();

    if (info != "")
        info = string(" ") + info;

    err->log(subject_fnc + string(" returned EGL_FALSE and generated err: ") +
                 int_to_hex_str(g_err) + info + string(" ") + gdev_info(dev) +
                 " caller_fnc: " + caller_fnc,
             TE_ERR_LOC);

    return false;
}

bool Gl_mgr::pro_egl_errs(Gdev *dev, string subject_fnc, string caller_fnc,
                          string info) {
    EGLint g_err = eglGetError();
    if (g_err == EGL_SUCCESS)
        return true;

    if (info != "")
        info = string(" ") + info;

    err->log(subject_fnc + string(" returned EGLint: ") +
                 int_to_hex_str(g_err) + info + string(" ") + gdev_info(dev) +
                 " caller_fnc: " + caller_fnc,
             TE_ERR_LOC);

    return false;
}

/**
 * If dev is null "app" extensions are checked (ex. EGL_MESA_platform_gbm)
 * If dev is not null "display" extensions are checked (ex. EGL_KHR_image_base)
 */
void Gl_mgr::check_egl_extensions(Gdev *dev) {
    Log_lvl log_lvl = err->get_logger()->get_log_lvl();
    string type;
    std::vector<string> vec;
    string exts;

    if (dev == nullptr) {
        const char *app_extensions =
            eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (!app_extensions)
            throw Run_err("EGL app_extensions does not exist", TE_ERR_LOC);
        exts = app_extensions;
        type = "app";

        /* extensions to check for */
        vec.emplace_back("EGL_MESA_platform_gbm");
    } else {
        if (dev->eg_dev.egl_dis == nullptr ||
            dev->eg_dev.egl_dis == EGL_NO_DISPLAY)
            throw Run_err("dev->eg_dev.egl_dis is null or EGL_NO_DISPLAY, " +
                              gdev_info(dev),
                          TE_ERR_LOC);
        Gdev *dev = gdev_mgr->devs.front().get();
        const char *display_extensions =
            eglQueryString(dev->eg_dev.egl_dis, EGL_EXTENSIONS);
        if (!display_extensions)
            throw Run_err("EGL display_extensions does not exist", TE_ERR_LOC);
        exts = display_extensions;
        type = "display";

        /* extensions to check for */
        vec.emplace_back("EGL_KHR_image_base");
        vec.emplace_back("EGL_MESA_image_dma_buf_export");
        vec.emplace_back("EGL_EXT_image_dma_buf_import");
    }

    if (log_lvl == Log_lvl::info) {
        string str1 = "Checking availability of EGL " + type + " extensions";
        if (dev)
            str1 += " for " + dev->dev_f->path;
        err->log(str1 + ":", TE_CUR_LOC);
    }

    for (auto &sub_str : vec) {
        if (exts.find(sub_str) == std::string::npos)
            throw Run_err(sub_str + " was not found in EGL " + type +
                              " extensions",
                          TE_ERR_LOC);
        else if (log_lvl == Log_lvl::info)
            err->log(sub_str, TE_CUR_LOC);
    }

    if (log_lvl == Log_lvl::info)
        err->log("EGL " + type + " extension checks are done.", TE_CUR_LOC);
}

void Gl_mgr::check_gl_extensions(Gdev *dev) {
    if (!dev)
        throw Arg_err("dev is null", TE_ERR_LOC);

    Log_lvl log_lvl = err->get_logger()->get_log_lvl();
    if (log_lvl == Log_lvl::info)
        err->log("Checking availability of GL extensions for " +
                     dev->dev_f->path + ":",
                 TE_CUR_LOC);

    std::vector<string> vec = {"GL_OES_EGL_image"};
    std::vector<string> exts;
    GLint ext_num = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ext_num);
    if (!pro_gl_errs(err, dev->dev_f->path, "glGetIntegerv GL_NUM_EXTENSIONS",
                     TE_ERR_LOC))
        throw Run_err("glGetIntegerv GL_NUM_EXTENSIONS err", TE_ERR_LOC);
    for (int i = 0; i < ext_num; ++i)
        exts.emplace_back((const char *)glGetStringi(GL_EXTENSIONS, i));

    for (string &str : vec) {
        if (std::find(exts.begin(), exts.end(), str) == exts.end())
            throw Run_err("GL extenion: " + str + " not found", TE_ERR_LOC);
        else if (log_lvl == Log_lvl::info)
            err->log(str, TE_ERR_LOC);
    }

    if (log_lvl == Log_lvl::info)
        err->log("GL extension checks are done.", TE_CUR_LOC);
}

void Gl_mgr::check_state(Gdev *dev) {
    if (!dev)
        throw Arg_err("dev is null", TE_ERR_LOC);

    EGLint value;
    /* make sure we have back buffer capability */
    EGLint expect = EGL_BACK_BUFFER;
    EGLBoolean ret =
        eglQueryContext(dev->eg_dev.egl_dis, dev->eg_dev.egl_context,
                        EGL_RENDER_BUFFER, &value);
    if (!pro_egl_errs(dev, "eglQueryContext for EGL_BACK_BUFFER", ret,
                      TE_ERR_LOC))
        throw Run_err("eglQueryContext failed for EGL_BACK_BUFFER", TE_ERR_LOC);
    if (value != expect)
        throw Run_err("eglQueryContext expected EGL_BACK_BUFFER " +
                          int_to_hex_str(expect) +
                          " got: " + int_to_hex_str(value),
                      TE_ERR_LOC);

    ret = eglQuerySurface(dev->eg_dev.egl_dis, dev->eg_dev.egl_sur,
                          EGL_RENDER_BUFFER, &value);
    if (!pro_egl_errs(dev, "eglQuerySurface for EGL_BACK_BUFFER", ret,
                      TE_ERR_LOC))
        throw Run_err("eglQuerySurface failed for EGL_BACK_BUFFER", TE_ERR_LOC);
    if (value != expect)
        throw Run_err("eglQuerySurface expected EGL_BACK_BUFFER " +
                          int_to_hex_str(expect) +
                          " got: " + int_to_hex_str(value),
                      TE_ERR_LOC);
}

} /* namespace Teave */