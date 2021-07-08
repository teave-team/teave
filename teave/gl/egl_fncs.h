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
#ifndef TEAVE_EGL_FNCS_H
#define TEAVE_EGL_FNCS_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

namespace Teave {

struct Egl_fncs {
    Egl_fncs();

    /** extension functions - we use eglGetProcAddress to get their address **/
    /* eglCreateImageKHR */
    PFNEGLCREATEIMAGEKHRPROC egl_create_image = nullptr;
    /* eglDestroyImageKHR */
    PFNEGLDESTROYIMAGEKHRPROC egl_destroy_image = nullptr;
    /* eglExportDMABUFImageQuery */
    PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC egl_export_dma_buf_img_query = nullptr;
    /* eglExportDMABUFImageMESA */
    PFNEGLEXPORTDMABUFIMAGEMESAPROC egl_export_dma_buf_img = nullptr;
    /* glEGLImageTargetTexture2DOES */
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC gl_egl_img_tar_text = nullptr;
    /* glEGLImageTargetRenderbufferStorageOES */
    PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC
        gl_egl_img_tar_rend_buf_storage = nullptr;
};

} // namespace Teave

#endif /* TEAVE_EGL_FNCS_H */
