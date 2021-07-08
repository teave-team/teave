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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/test)
set(TEST_INSTALL_HEADERS
    ${HEADER_LOC}/fixture.h
    ${HEADER_LOC}/util.h
    PARENT_SCOPE
)

SET(TEST_TARGET_SRC
    fixture.cpp
    util.cpp
    )

add_library(TEST_OBJ OBJECT ${TEST_TARGET_SRC})