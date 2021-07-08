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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/compositor)
set(COMPOSITOR_INSTALL_HEADERS
    ${HEADER_LOC}/compositor.h
    
    PARENT_SCOPE
)
SET(COMPOSITOR_SRC
    compositor.cpp
    )

add_library(COMPOSITOR_OBJ OBJECT ${COMPOSITOR_SRC})