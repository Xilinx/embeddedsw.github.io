/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xwdtps_sinit.c
* @addtogroup wdtps Overview
* @{
*
* This file contains method for static initialization (compile-time) of the
* driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ----------------------------------------------
* 1.00a ecm/jz 01/15/10 First release
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.6   sb     06/27/23 Added support for system device-tree flow.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xwdtps.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* Lookup the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param	DeviceId is the unique device ID of the device being looked up.
*
* @return	A pointer to the configuration table entry corresponding to the
*		given device ID, or NULL if no match is found.
*
* @note		None.
*
******************************************************************************/
#ifndef SDT
XWdtPs_Config *XWdtPs_LookupConfig(u16 DeviceId)
{
	XWdtPs_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_XWDTPS_NUM_INSTANCES; Index++) {
		if (XWdtPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XWdtPs_ConfigTable[Index];
			break;
		}
	}
	return (XWdtPs_Config *)CfgPtr;
}
#else
XWdtPs_Config *XWdtPs_LookupConfig(u32 BaseAddress)
{
	XWdtPs_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; XWdtPs_ConfigTable[Index].Name != NULL; Index++) {
		if ((XWdtPs_ConfigTable[Index].BaseAddress == BaseAddress) ||
		    !BaseAddress) {
			CfgPtr = &XWdtPs_ConfigTable[Index];
			break;
		}
	}
	return (XWdtPs_Config *)CfgPtr;
}
#endif
/** @} */
