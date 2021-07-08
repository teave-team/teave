#
# Copyright 2021 Pedram Tiv
#
# Licensed under the Teave Public License (the "License");
# You may obtain a copy of the License at https://www.teave.io/teave-public-license.html
#
# This software is distributed under the License on an "AS IS" BASIS, WITHOUT WARRANTIES 
# OF ANY KIND, either express or implied. See the License for the specific language governing 
# terms and conditions for use, reproduction, and distribution.
#
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/gl)
set(GL_INSTALL_HEADERS
    ${HEADER_LOC}/egl_fncs.h
    ${HEADER_LOC}/gl_include.h
    ${HEADER_LOC}/gl_mgr.h
    ${HEADER_LOC}/gl_util.h

    PARENT_SCOPE
)

SET(GL_TARGET_SRC
    egl_fncs.cpp
    gl_mgr.cpp
    gl_util.cpp
    )

add_library(GL_OBJ OBJECT ${GL_TARGET_SRC})
