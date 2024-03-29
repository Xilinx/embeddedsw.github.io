# Copyright (c) 2021 - 2022 Xilinx, Inc.  All rights reserved.
# Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.14.7)
include(${CMAKE_CURRENT_SOURCE_DIR}/AxiethernetExample.cmake)
project(axiethernet)

find_package(common)
enable_language(C ASM)
collect(PROJECT_LIB_DEPS xilstandalone)
collect(PROJECT_LIB_DEPS xil)
collect(PROJECT_LIB_DEPS xiltimer)
collect(PROJECT_LIB_DEPS gcc)
collect(PROJECT_LIB_DEPS c)
collector_list (_deps PROJECT_LIB_DEPS)

SET(DRIVER_INSTANCES "${NUM_DRIVER_INSTANCES}" CACHE STRING "Driver Instances")
SET_PROPERTY(CACHE DRIVER_INSTANCES PROPERTY STRINGS "${NUM_DRIVER_INSTANCES}")
set(index 0)
LIST_INDEX(${index} ${DRIVER_INSTANCES} "${NUM_DRIVER_INSTANCES}")
list(GET TOTAL_EXAMPLE_LIST ${index} ex_list)
list(GET REG_LIST ${index} reg)
SET(XAXIETHERNET_BASEADDRESS "${reg}")
SET(COMMON_FILE xaxiethernet_example_util.c)
SET(EXAMPLE_LIST "${${ex_list}}" CACHE STRING "Driver Example List")
SET_PROPERTY(CACHE EXAMPLE_LIST PROPERTY STRINGS "${${ex_list}}")

set(valid_ex 0)
foreach(LIST1 ${TOTAL_EXAMPLE_LIST})
    if ("${${LIST1}}" STREQUAL "${EXAMPLE_LIST}")
	set(valid_ex 1)
	break()
    endif()
endforeach()

set(CMAKE_INFILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../scripts/linker_files/")
linker_gen(${CMAKE_INFILE_PATH})
gen_exheader(${CMAKE_INFILE_PATH} ${CMAKE_PROJECT_NAME} ${XAXIETHERNET_BASEADDRESS} "")

if (${valid_ex})
    list(APPEND COMMON_EXAMPLES "${${ex_list}}")
else()
    list(APPEND COMMON_EXAMPLES ${EXAMPLE_LIST})
endif()

foreach(EXAMPLE ${COMMON_EXAMPLES})
    string(REGEX REPLACE "\\.[^.]*$" "" EXAMPLE ${EXAMPLE})
    add_executable(${EXAMPLE}.elf ${EXAMPLE} ${COMMON_FILE})
    target_link_libraries(${EXAMPLE}.elf -Wl,--gc-sections -T\"${CMAKE_SOURCE_DIR}/lscript.ld\" -L\"${CMAKE_SOURCE_DIR}/\" -Wl,--start-group ${_deps} -Wl,--end-group)
endforeach()
