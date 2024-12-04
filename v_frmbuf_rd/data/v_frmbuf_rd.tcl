#############################################################################
# Copyright (C) 2017-2023 Xilinx, Inc. All rights reserved
# Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
# SPDX-License-Identifier: MIT
##
#############################################################################

proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XV_frmbufrd" "NUM_INSTANCES" "DEVICE_ID" "C_S_AXI_CTRL_BASEADDR" "C_S_AXI_CTRL_HIGHADDR" "SAMPLES_PER_CLOCK" "MAX_COLS" "MAX_ROWS" "MAX_DATA_WIDTH" "AXIMM_DATA_WIDTH" "AXIMM_ADDR_WIDTH" "HAS_RGBX8" "HAS_YUVX8" "HAS_YUYV8" "HAS_RGBA8" "HAS_YUVA8" "HAS_RGBX10" "HAS_YUVX10" "HAS_Y_UV8" "HAS_Y_UV8_420" "HAS_RGB8" "HAS_YUV8" "HAS_Y_UV10" "HAS_Y_UV10_420" "HAS_Y8" "HAS_Y10" "HAS_BGRA8" "HAS_BGRX8" "HAS_UYVY8"  "HAS_BGR8" "HAS_RGBX12" "HAS_RGB16" "HAS_YUVX12" "HAS_Y_UV12" "HAS_Y_UV12_420" "HAS_Y12" "HAS_YUV16" "HAS_Y_UV16" "HAS_Y_UV16_420" "HAS_Y16" "HAS_Y_U_V8" "HAS_Y_U_V10" "HAS_Y_U_V8_420" "HAS_Y_U_V12" "HAS_INTERLACED" "IS_TILE_FORMAT"

    xdefine_config_file $drv_handle "xv_frmbufrd_g.c" "XV_frmbufrd" "DEVICE_ID" "C_S_AXI_CTRL_BASEADDR" "SAMPLES_PER_CLOCK" "MAX_COLS" "MAX_ROWS" "MAX_DATA_WIDTH" "AXIMM_DATA_WIDTH" "AXIMM_ADDR_WIDTH" "HAS_RGBX8" "HAS_YUVX8" "HAS_YUYV8" "HAS_RGBA8" "HAS_YUVA8" "HAS_RGBX10" "HAS_YUVX10" "HAS_Y_UV8" "HAS_Y_UV8_420" "HAS_RGB8" "HAS_YUV8" "HAS_Y_UV10" "HAS_Y_UV10_420" "HAS_Y8" "HAS_Y10" "HAS_BGRA8" "HAS_BGRX8" "HAS_UYVY8" "HAS_BGR8" "HAS_RGBX12" "HAS_RGB16" "HAS_YUVX12" "HAS_Y_UV12" "HAS_Y_UV12_420" "HAS_Y12" "HAS_YUV16" "HAS_Y_UV16" "HAS_Y_UV16_420" "HAS_Y16" "HAS_Y_U_V8" "HAS_Y_U_V10" "HAS_Y_U_V8_420" "HAS_Y_U_V12" "HAS_INTERLACED" "IS_TILE_FORMAT"

    xdefine_canonical_xpars $drv_handle "xparameters.h" "XV_frmbufrd" "DEVICE_ID" "C_S_AXI_CTRL_BASEADDR" "C_S_AXI_CTRL_HIGHADDR" "SAMPLES_PER_CLOCK" "MAX_COLS" "MAX_ROWS" "MAX_DATA_WIDTH" "AXIMM_DATA_WIDTH" "AXIMM_ADDR_WIDTH" "HAS_RGBX8" "HAS_YUVX8" "HAS_YUYV8" "HAS_RGBA8" "HAS_YUVA8" "HAS_RGBX10" "HAS_YUVX10" "HAS_Y_UV8" "HAS_Y_UV8_420" "HAS_RGB8" "HAS_YUV8" "HAS_Y_UV10" "HAS_Y_UV10_420" "HAS_Y8" "HAS_Y10" "HAS_BGRA8" "HAS_BGRX8" "HAS_UYVY8"  "HAS_BGR8" "HAS_RGBX12" "HAS_RGB16" "HAS_YUVX12" "HAS_Y_UV12" "HAS_Y_UV12_420" "HAS_Y12" "HAS_YUV16" "HAS_Y_UV16" "HAS_Y_UV16_420" "HAS_Y16" "HAS_Y_U_V8" "HAS_Y_U_V10" "HAS_Y_U_V8_420" "HAS_Y_U_V12" "HAS_INTERLACED" "IS_TILE_FORMAT"
}
