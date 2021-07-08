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
#include "egl_fncs.h"

#include <teave/err/err.h>

namespace Teave {

Egl_fncs::Egl_fncs() {
    egl_create_image = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(
        eglGetProcAddress("eglCreateImageKHR"));
    if (!egl_create_image)
        throw Run_err("eglGetProcAddress(eglCreateImageKHR) returned null",
                      TE_ERR_LOC);

    egl_destroy_image = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(
        eglGetProcAddress("eglDestroyImageKHR"));
    if (!egl_destroy_image)
        throw Run_err("eglGetProcAddress(eglDestroyImageKHR) returned null",
                      TE_ERR_LOC);

    egl_export_dma_buf_img_query =
        reinterpret_cast<PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC>(
            eglGetProcAddress("eglExportDMABUFImageQueryMESA"));
    if (!egl_export_dma_buf_img_query)
        throw Run_err(
            "eglGetProcAddress(eglExportDMABUFImageQueryMESA) returned null",
            TE_ERR_LOC);

    egl_export_dma_buf_img = reinterpret_cast<PFNEGLEXPORTDMABUFIMAGEMESAPROC>(
        eglGetProcAddress("eglExportDMABUFImageMESA"));
    if (!egl_export_dma_buf_img)
        throw Run_err(
            "eglGetProcAddress(eglExportDMABUFImageMESA) returned null",
            TE_ERR_LOC);

    gl_egl_img_tar_text = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    if (!gl_egl_img_tar_text)
        throw Run_err(
            "eglGetProcAddress(glEGLImageTargetTexture2DOES) returned null",
            TE_ERR_LOC);

    gl_egl_img_tar_rend_buf_storage =
        reinterpret_cast<PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC>(
            eglGetProcAddress("glEGLImageTargetRenderbufferStorageOES"));
    if (!gl_egl_img_tar_rend_buf_storage)
        throw Run_err("eglGetProcAddress("
                      "glEGLImageTargetRenderbufferStorageOES) returned null",
                      TE_ERR_LOC);
}

} // namespace Teave