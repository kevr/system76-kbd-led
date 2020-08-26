#
# cg-sensor's CMake root instructions.
# 
# Copyright (C) 2020 Kevin Morris
# All Rights Reserved.
#
cmake_minimum_required(VERSION 2.8.8)
project(system76-kbd-led)

# We require a C++17 compliant compiler
set(
    CMAKE_CXX_FLAGS_DEBUG
    "-O0 -g -std=c++20"
)
set(
    CMAKE_CXX_FLAGS_RELEASE
    "-O3 -std=c++20"
)

# Setup library flags for all C++ targets.
# set(PROJECT_LIBS "-pthread")
# set(CMAKE_CXX_STANDARD_LIBRARIES ${PROJECT_LIBS})

add_subdirectory(src)

install(
    FILES
    systemd/system76-kbd-led.service
    systemd/system76-kbd-led-cache.service
    DESTINATION
    "lib/systemd/system"
)

execute_process(
    COMMAND
    bash -c "git describe --long | sed 's/\\([^a-z]*\\)-g.*/\\1/;s/-/./g' | tr -d '\\n'"
    OUTPUT_VARIABLE PKG_VERSION
)

message(STATUS "PKG_VERSION: '${PKG_VERSION}'")

set(CPACK_PACKAGE_VERSION "${PKG_VERSION}")
set(CPACK_DEBIAN_PACKAGE_VERSION "${PKG_VERSION}")
set(CPACK_RPM_PACKAGE_VERSION ${CPACK_DEBIAN_PACKAGE_VERSION})

set(CPACK_PACKAGE_CONTACT "kevr.gtalk@gmail.com")
include(CPack)
