/******************************************************************************
* Copyright (C) 2017 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc.  All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xuartpsv_sinit.c
* @addtogroup uartpsv Overview
* @{
*
* The xuartpsv_sinit.c file contains the implementation of the XUartPsv driver's
* static initialization functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver  Who  Date      Changes
* ---  ---  --------- -----------------------------------------------
* 1.0  sg   09/18/17  First Release
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#ifndef SDT
#include "xparameters.h"
#endif
#include "xuartpsv.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param	DeviceId contains the ID of the device
*
* @return	A pointer to the configuration structure or NULL if the
*		specified device is not in the system.
*
* @note 	None.
*
******************************************************************************/
#ifndef SDT
XUartPsv_Config *XUartPsv_LookupConfig(u16 DeviceId)
{
	XUartPsv_Config *CfgPtr = NULL;

	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_XUARTPSV_NUM_INSTANCES; Index++) {
		if (XUartPsv_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XUartPsv_ConfigTable[Index];
			break;
		}
	}

	return (XUartPsv_Config *)CfgPtr;
}
#else
XUartPsv_Config *XUartPsv_LookupConfig(UINTPTR BaseAddress)
{
	XUartPsv_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; XUartPsv_ConfigTable[Index].Name != NULL; Index++) {
		if ((XUartPsv_ConfigTable[Index].BaseAddress == BaseAddress) ||
		    !BaseAddress) {
			CfgPtr = &XUartPsv_ConfigTable[Index];
			break;
		}
	}

	return (XUartPsv_Config *)CfgPtr;
}
#endif
/** @} */
