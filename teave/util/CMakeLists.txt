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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/util)
set(UTIL_INSTALL_HEADERS
    ${HEADER_LOC}/file.h
    ${HEADER_LOC}/time.h
    ${HEADER_LOC}/ui.h
    ${HEADER_LOC}/util.h
    ${HEADER_LOC}/uuid_gen.h

    PARENT_SCOPE
)

SET(UTIL_TARGET_SRC    
    file.cpp
    time.cpp
    ui.cpp
    util.cpp
    uuid_gen.cpp
    )

add_library(UTIL_OBJ OBJECT ${UTIL_TARGET_SRC})
