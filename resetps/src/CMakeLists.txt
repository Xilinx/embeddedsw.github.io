# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.15)
project(resetps)

find_package(common)
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)
collect (PROJECT_LIB_HEADERS xresetps.h)
collect (PROJECT_LIB_SOURCES xresetps_g.c)
collect (PROJECT_LIB_SOURCES xresetps.c)
collect (PROJECT_LIB_HEADERS xresetps_hw.h)
collect (PROJECT_LIB_SOURCES xresetps_sinit.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(resetps STATIC ${_sources})
set_target_properties(resetps PROPERTIES LINKER_LANGUAGE C)
