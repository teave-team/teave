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
set(HEADER_LOC ${PROJECT_SOURCE_DIR}/event)
set(EVENT_INSTALL_HEADERS
    ${HEADER_LOC}/ev_mgr.h
    ${HEADER_LOC}/event.h
    ${HEADER_LOC}/signal_handler.h
    PARENT_SCOPE
)
SET(EVENT_SRC
    event.cpp
    ev_mgr.cpp
    signal_handler.cpp
    )

add_library(EVENT_OBJ OBJECT ${EVENT_SRC})