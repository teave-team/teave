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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/gdev)
set(GDEV_INSTALL_HEADERS
    ${HEADER_LOC}/drm.h
    ${HEADER_LOC}/gdev.h
    ${HEADER_LOC}/gdev_helpers.h
    ${HEADER_LOC}/gdev_mgr.h

    PARENT_SCOPE
)
SET(GDEV_SRC
    gdev_mgr.cpp    
    gdev.cpp
    gdev_helpers.cpp
    drm.cpp
    )

add_library(GDEV_OBJ OBJECT ${GDEV_SRC})