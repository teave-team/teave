# Minimum CMake version
cmake_minimum_required(VERSION 3.13)

# -fPIC: for creating shared libaray. if not included
# we get this err relocation R_X86_64_32 against `.rodata' can not be used when making a shared object
SET(CMAKE_CXX_FLAGS "-std=c++17 -Wall -fPIC")

# Project
set(TE_PROJECT_NAME teave)
set(TE_SV_NAME teave-sv)
set(TE_TESTS_NAME tests)
project(${TE_PROJECT_NAME} CXX)

# -Wl -export-dynamic, for backtrace - (dev only: remove for prod)
# -lc++fs: for filesystem
# note, the order matters
SET(TE_LINK_FLAGS
-Wl,--export-dynamic
-ldl
-lstdc++fs
)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# path variables #
# project root path
# assuming parent dir of current dir is project-root
get_filename_component(TE_ROOT_DIR ../ ABSOLUTE CACHE)
# path to build directory
set(TE_BUILD_DIR ${TE_ROOT_DIR}/build)
# path to source files
set(PROJECT_SOURCE_DIR ${TE_ROOT_DIR}/teave)
# path to cmake src dir in build dir
set(TE_BUILD_SRC_DIR ${TE_BUILD_DIR}/teave)
# path to teave-sv source
set(TE_TEAVE_SV_SOURCE_DIR ${TE_ROOT_DIR}/teave-sv)
# path to cmake teave-sv dir in build dir
set(TE_TEAVE_SV_BUILD_SRC_DIR ${TE_BUILD_DIR}/teave-sv)
# path to test source
set(TE_TEST_SOURCE_DIR ${TE_ROOT_DIR}/test)
# path to cmake test dir in build dir
set(TE_TEST_BUILD_SRC_DIR ${TE_BUILD_DIR}/test)

# path to the executable(s)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TE_ROOT_DIR}/bin)

# install paths
set(TE_INSTALL_LIB /usr/lib/teave)
set(TE_INSTALL_BIN /usr/lib/teave)
set(TE_INSTALL_SV /etc/systemd/system)
set(TE_INSTALL_INCLUDE /usr/include/teave)

# Log destination
# dash separated list
# file: to log file set by TE_LOG_DIR and TE_SV_LOG_DIR
# syslog
# stderr
# stdout
# none
set(TE_LOG_DEST file-stderr)

# Log level (this is used by Teave::logger)
# none: no logs 
# err: only errors
# info: errors and info
set(TE_LOG_LVL err)

# User the App is running under must have
# write permission to this dir
set(TE_LOG_DIR $ENV{HOME}/teave-logs)
set(TE_LOG log)

# Teave-sv err log.
# Note,Teave-sv runs as root.
set(TE_SV_LOG_DIR $ENV{HOME}/teave-logs)
set(TE_SV_LOG sv_log)

# User the Test is running under must have
# write permission to this dir
set(TE_TEST_LOG_DIR $ENV{HOME}/teave-logs)
set(TE_TEST_LOG test_log)

# Mode
# prod: Teave-sv will only receive messages from the first 
#       process connecting to the socket
# dev: When a new process connects to Teave-sv, Teave-sv will close
#      the previous process' socket and receive from the new process
#       (this is to avoid the need to restart Teave-sv every time
#        app is restarted during development)
set(TE_MODE dev)

# include paths #
# note, ${PROJECT_SOURCE_DIR}/include is for included third party libraries
include_directories(BEFORE
    ${TE_ROOT_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/glm
)

include_directories(AFTER
    /usr/include/libdrm
    )

set(TE_LINK_LIBS
    /lib/x86_64-linux-gnu/libudev.so
    /usr/lib/x86_64-linux-gnu/libdrm.so 
    /usr/lib/x86_64-linux-gnu/libgbm.so
    /usr/lib/x86_64-linux-gnu/libEGL.so
    /usr/lib/x86_64-linux-gnu/libGLESv2.so
)


### preprocessors ###
# note, we need to manipulate the string constant to avoid expansion errors
# see src/commons.h for more details
# ex: TE_APP_NAME_ will be come TE_APP_NAME in src/commons.h
add_compile_definitions(TE_APP_NAME_=${TE_PROJECT_NAME})
add_compile_definitions(TE_LOG_DEST_=${TE_LOG_DEST})
add_compile_definitions(TE_LOG_LVL_=${TE_LOG_LVL})
add_compile_definitions(TE_LOG_FILE_=${TE_LOG_DIR}/${TE_LOG})
add_compile_definitions(TE_SV_LOG_FILE_=${TE_SV_LOG_DIR}/${TE_SV_LOG})
add_compile_definitions(TE_TEST_LOG_FILE_=${TE_TEST_LOG_DIR}/${TE_TEST_LOG})
add_compile_definitions(TE_MODE_=${TE_MODE})


### build targets  ###
# add sub dirs
add_subdirectory(${PROJECT_SOURCE_DIR} ${TE_BUILD_SRC_DIR})
add_subdirectory(${TE_TEAVE_SV_SOURCE_DIR} ${TE_TEAVE_SV_BUILD_SRC_DIR})

# Teave: link (order matters)
target_link_libraries(${TE_PROJECT_NAME} PUBLIC ${TE_LINK_FLAGS} ${TE_LINK_LIBS})
set_target_properties(${TE_PROJECT_NAME} PROPERTIES VERSION 0.1.1 SOVERSION 0.1)
# install 
install(TARGETS ${TE_PROJECT_NAME} LIBRARY DESTINATION ${TE_INSTALL_LIB})

# Teave-sv #
add_executable(${TE_SV_NAME} $<TARGET_OBJECTS:SV_WITH_MAIN_OBJ> ${SV_WITHOUT_MAIN_OBJS} ${TE_OBJS})
# note, the order matters. ${TE_LINK_FLAGS} must be right after PUBLIC
target_link_libraries(${TE_SV_NAME} PUBLIC ${TE_LINK_FLAGS} ${TE_LINK_LIBS})
install(PROGRAMS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TE_SV_NAME} DESTINATION ${TE_INSTALL_BIN})
install(PROGRAMS ${TE_TEAVE_SV_SOURCE_DIR}/${TE_SV_NAME}.service 
    DESTINATION ${TE_INSTALL_SV}
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

### Test setup ###
# Unit and Functional Tests #
# using Catch2 https://github.com/catchorg/Catch2
add_subdirectory(${TE_TEST_SOURCE_DIR} ${TE_TEST_BUILD_SRC_DIR})
add_executable(tests $<TARGET_OBJECTS:TE_TEST_OBJ> )
target_link_libraries(tests PUBLIC ${TE_LINK_FLAGS} ${TE_PROJECT_NAME} ${TE_LINK_LIBS})