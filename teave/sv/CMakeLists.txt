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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/sv)
set(SV_INSTALL_HEADERS
    ${HEADER_LOC}/msg.h
    ${HEADER_LOC}/sv.h
    ${HEADER_LOC}/sva.h
    ${HEADER_LOC}/svs.h
    ${HEADER_LOC}/util.h
    PARENT_SCOPE
)
SET(SV_SRC
    msg.cpp
    sv.cpp
    sva.cpp
    svs.cpp
    util.cpp
    )

add_library(SV_OBJ OBJECT ${SV_SRC})