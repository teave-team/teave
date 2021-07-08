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

add_subdirectory(${PROJECT_SOURCE_DIR}/compositor ${TE_BUILD_SRC_DIR}/compositor)
install(FILES ${COMPOSITOR_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/compositor)

add_subdirectory(${PROJECT_SOURCE_DIR}/debug ${TE_BUILD_SRC_DIR}/debug)
install(FILES ${DEBUG_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/debug)

add_subdirectory(${PROJECT_SOURCE_DIR}/device ${TE_BUILD_SRC_DIR}/device)
install(FILES ${DEVICE_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/device)

add_subdirectory(${PROJECT_SOURCE_DIR}/epoll ${TE_BUILD_SRC_DIR}/epoll)
install(FILES ${EPOLL_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/epoll)

add_subdirectory(${PROJECT_SOURCE_DIR}/err ${TE_BUILD_SRC_DIR}/err)
install(FILES ${ERR_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/err)

add_subdirectory(${PROJECT_SOURCE_DIR}/event ${TE_BUILD_SRC_DIR}/event)
install(FILES ${EVENT_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/event)

add_subdirectory(${PROJECT_SOURCE_DIR}/gdev ${TE_BUILD_SRC_DIR}/gdev)
install(FILES ${GDEV_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/gdev)

add_subdirectory(${PROJECT_SOURCE_DIR}/gl ${TE_BUILD_SRC_DIR}/gl)
install(FILES ${GL_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/gl)

# third party libs
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION ${TE_INSTALL_INCLUDE})

add_subdirectory(${PROJECT_SOURCE_DIR}/setting ${TE_BUILD_SRC_DIR}/setting)
install(FILES ${SETTING_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/setting)

add_subdirectory(${PROJECT_SOURCE_DIR}/socket ${TE_BUILD_SRC_DIR}/socket)
install(FILES ${SOCKET_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/socket)

add_subdirectory(${PROJECT_SOURCE_DIR}/sv ${TE_BUILD_SRC_DIR}/sv)
install(FILES ${SV_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/sv)

add_subdirectory(${PROJECT_SOURCE_DIR}/logger ${TE_BUILD_SRC_DIR}/logger)
install(FILES ${LOGGER_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/logger)

add_subdirectory(${PROJECT_SOURCE_DIR}/runtime ${TE_BUILD_SRC_DIR}/runtime)
install(FILES ${RUNTIME_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/runtime)

add_subdirectory(${PROJECT_SOURCE_DIR}/test ${TE_BUILD_SRC_DIR}/test)
install(FILES ${TEST_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/test)

add_subdirectory(${PROJECT_SOURCE_DIR}/tty ${TE_BUILD_SRC_DIR}/tty)
install(FILES ${TTY_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/tty)

add_subdirectory(${PROJECT_SOURCE_DIR}/user ${TE_BUILD_SRC_DIR}/user)
install(FILES ${USER_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/user)

add_subdirectory(${PROJECT_SOURCE_DIR}/util ${TE_BUILD_SRC_DIR}/util)
install(FILES ${UTIL_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE}/util)

# files in top folder
SET(_SRC
    common.cpp
    compile_check.cpp
)
SET(_INSTALL_HEADERS
    common.h
)
add_library(_OBJ OBJECT ${_SRC})
install(FILES ${_INSTALL_HEADERS} DESTINATION ${TE_INSTALL_INCLUDE})

SET(TE_OBJS_
    $<TARGET_OBJECTS:_OBJ>
    $<TARGET_OBJECTS:COMPOSITOR_OBJ>
    $<TARGET_OBJECTS:DEBUG_OBJ>
    $<TARGET_OBJECTS:DEVICE_OBJ>    
    $<TARGET_OBJECTS:EPOLL_OBJ>
    $<TARGET_OBJECTS:ERR_OBJ>
    $<TARGET_OBJECTS:EVENT_OBJ>
    $<TARGET_OBJECTS:GDEV_OBJ>
    $<TARGET_OBJECTS:GL_OBJ>
    $<TARGET_OBJECTS:LOGGER_OBJ>
    $<TARGET_OBJECTS:RUNTIME_OBJ>
    $<TARGET_OBJECTS:SETTING_OBJ>
    $<TARGET_OBJECTS:SOCKET_OBJ>
    $<TARGET_OBJECTS:SV_OBJ>
    $<TARGET_OBJECTS:TEST_OBJ>
    $<TARGET_OBJECTS:TTY_OBJ>
    $<TARGET_OBJECTS:USER_OBJ>
    $<TARGET_OBJECTS:UTIL_OBJ>
)
SET(TE_OBJS ${TE_OBJS_} PARENT_SCOPE)

add_library(${TE_PROJECT_NAME} SHARED ${TE_OBJS_})