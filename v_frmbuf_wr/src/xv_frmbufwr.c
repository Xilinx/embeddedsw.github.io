// ==============================================================
// Copyright (c) 1986 - 2021 Xilinx Inc. All rights reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// SPDX-License-Identifier: MIT
// ==============================================================

/***************************** Include Files *********************************/
#include "xv_frmbufwr.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XV_frmbufwr_CfgInitialize(XV_frmbufwr *InstancePtr,
                               XV_frmbufwr_Config *ConfigPtr,
                               UINTPTR EffectiveAddr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);
    Xil_AssertNonvoid(EffectiveAddr != (UINTPTR)NULL);

    /* Setup the instance */
    InstancePtr->Config = *ConfigPtr;
    InstancePtr->Config.BaseAddress = EffectiveAddr;
#ifdef SDT
    InstancePtr->Config.IntrId = ConfigPtr->IntrId;
    InstancePtr->Config.IntrParent = ConfigPtr->IntrParent;
#endif

    /* Set the flag to indicate the driver is ready */
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XV_frmbufwr_Start(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL) & 0x80;
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XV_frmbufwr_IsDone(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XV_frmbufwr_IsIdle(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XV_frmbufwr_IsReady(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XV_frmbufwr_EnableAutoRestart(XV_frmbufwr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL, 0x80);
}

void XV_frmbufwr_DisableAutoRestart(XV_frmbufwr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL, 0);
}

void XV_frmbufwr_SetFlushbit(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress,
			       XV_FRMBUFWR_CTRL_ADDR_AP_CTRL);
    Data |= XV_FRMBUFWR_CTRL_BITS_FLUSH_BIT;
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress,
			 XV_FRMBUFWR_CTRL_ADDR_AP_CTRL, Data);
}

u32 XV_frmbufwr_Get_FlushDone(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_AP_CTRL)
			       & XV_FRMBUFWR_CTRL_BITS_FLUSH_STATUSBIT;
    return Data;
}

void XV_frmbufwr_Set_HwReg_width(XV_frmbufwr *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_WIDTH_DATA, Data);
}

u32 XV_frmbufwr_Get_HwReg_width(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_WIDTH_DATA);
    return Data;
}

void XV_frmbufwr_Set_HwReg_height(XV_frmbufwr *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_HEIGHT_DATA, Data);
}

u32 XV_frmbufwr_Get_HwReg_height(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_HEIGHT_DATA);
    return Data;
}

void XV_frmbufwr_Set_HwReg_stride(XV_frmbufwr *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_STRIDE_DATA, Data);
}

u32 XV_frmbufwr_Get_HwReg_stride(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_STRIDE_DATA);
    return Data;
}

void XV_frmbufwr_Set_HwReg_video_format(XV_frmbufwr *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_VIDEO_FORMAT_DATA, Data);
}

u32 XV_frmbufwr_Get_HwReg_video_format(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_VIDEO_FORMAT_DATA);
    return Data;
}

void XV_frmbufwr_Set_HwReg_frm_buffer_V(XV_frmbufwr *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER_V_DATA, (u32)(Data));
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER_V_DATA + 4, (u32)(Data >> 32));
}

u64 XV_frmbufwr_Get_HwReg_frm_buffer_V(XV_frmbufwr *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER_V_DATA);
    Data += (u64)XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER_V_DATA + 4) << 32;
    return Data;
}

void XV_frmbufwr_Set_HwReg_frm_buffer2_V(XV_frmbufwr *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER2_V_DATA, (u32)(Data));
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER2_V_DATA + 4, (u32)(Data >> 32));
}

u64 XV_frmbufwr_Get_HwReg_frm_buffer2_V(XV_frmbufwr *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER2_V_DATA);
    Data += (u64)XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER2_V_DATA + 4) << 32;
    return Data;
}

void XV_frmbufwr_Set_HwReg_frm_buffer3_V(XV_frmbufwr *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER3_V_DATA, (u32)(Data));
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER3_V_DATA + 4, (u32)(Data >> 32));
}

u64 XV_frmbufwr_Get_HwReg_frm_buffer3_V(XV_frmbufwr *InstancePtr) {
    u64 Data;
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER3_V_DATA);
    Data += (u64)XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FRM_BUFFER3_V_DATA + 4) << 32;
    return Data;
}

u32 XV_frmbufwr_Get_HwReg_field_id(XV_frmbufwr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    Xil_AssertNonvoid(InstancePtr->Config.Interlaced);

    Data = XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_HWREG_FIELD_ID_DATA);
    return Data;
}

void XV_frmbufwr_InterruptGlobalEnable(XV_frmbufwr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_GIE, 1);
}

void XV_frmbufwr_InterruptGlobalDisable(XV_frmbufwr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_GIE, 0);
}

void XV_frmbufwr_InterruptEnable(XV_frmbufwr *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_IER);
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_IER, Register | Mask);
}

void XV_frmbufwr_InterruptDisable(XV_frmbufwr *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_IER);
    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_IER, Register & (~Mask));
}

void XV_frmbufwr_InterruptClear(XV_frmbufwr *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XV_frmbufwr_WriteReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_ISR, Mask);
}

u32 XV_frmbufwr_InterruptGetEnabled(XV_frmbufwr *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_IER);
}

u32 XV_frmbufwr_InterruptGetStatus(XV_frmbufwr *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XV_frmbufwr_ReadReg(InstancePtr->Config.BaseAddress, XV_FRMBUFWR_CTRL_ADDR_ISR);
}
