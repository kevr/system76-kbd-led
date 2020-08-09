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
    "-O0 -g"
)
set(
    CMAKE_CXX_FLAGS_RELEASE
    "-O3"
)

# Setup library flags for all C++ targets.
# set(PROJECT_LIBS "-pthread")
# set(CMAKE_CXX_STANDARD_LIBRARIES ${PROJECT_LIBS})

add_subdirectory(src)

set(CPACK_PACKAGE_CONTACT "kevr.gtalk@gmail.com")
include(CPack)
