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
SET(SV_BLD_DIR ${TE_TEAVE_SV_BUILD_SRC_DIR})

add_subdirectory(dev ${SV_BLD_DIR}/dev)
add_subdirectory(mgr ${SV_BLD_DIR}/mgr)

add_library(SV_WITH_MAIN_OBJ OBJECT main.cpp)

SET(SV_WITHOUT_MAIN_OBJS
    $<TARGET_OBJECTS:SV_DEV_OBJ>
    $<TARGET_OBJECTS:SV_MGR_OBJ>
    
    PARENT_SCOPE
)