# Copyright (C) 2023 - 2025 Advanced Micro Devices, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.15)
project(tmr_manager)

find_package(common)
enable_language(C ASM CXX)
collector_create (PROJECT_LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}")
collector_create (PROJECT_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_BINARY_DIR}/include)
collect (PROJECT_LIB_SOURCES xtmr_manager.c)
collect (PROJECT_LIB_HEADERS xtmr_manager.h)
collect (PROJECT_LIB_SOURCES xtmr_manager_g.c)
collect (PROJECT_LIB_HEADERS xtmr_manager_i.h)
collect (PROJECT_LIB_SOURCES xtmr_manager_intr.c)
collect (PROJECT_LIB_HEADERS xtmr_manager_l.h)
collect (PROJECT_LIB_SOURCES xtmr_manager_recover.c)
collect (PROJECT_LIB_SOURCES xtmr_manager_recover_l.S)
collect (PROJECT_LIB_SOURCES xtmr_manager_selftest.c)
collect (PROJECT_LIB_SOURCES xtmr_manager_sem.c)
collect (PROJECT_LIB_SOURCES xtmr_manager_sinit.c)
collect (PROJECT_LIB_SOURCES xtmr_manager_stats.c)
collector_list (_sources PROJECT_LIB_SOURCES)
collector_list (_headers PROJECT_LIB_HEADERS)
file(COPY ${_headers} DESTINATION ${CMAKE_BINARY_DIR}/include)
add_library(tmr_manager STATIC ${_sources})
set_target_properties(tmr_manager PROPERTIES LINKER_LANGUAGE C)
