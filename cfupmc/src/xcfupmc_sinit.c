/******************************************************************************
* Copyright (C) 2017 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xcfupmc_sinit.c
* @addtogroup cfupmc_v1_1
* @{
*
* This file contains static initialization methods for Xilinx CFU core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- ---------------------------------------------------
* 1.0   kc   22/10/17 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xcfupmc.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/


/**************************** Type Definitions *******************************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
*
* XCfupmc_LookupConfig returns a reference to an XCfupmc_Config structure
* based on the unique device id, <i>DeviceId</i>. The return value will refer
* to an entry in the device configuration table defined in the xcfupmc_g.c
* file.
*
* @param	DeviceId is the unique device ID of the device for the lookup
*		operation.
*
* @return	CfgPtr is a reference to a config record in the configuration
*		table (in xcfupmc_g.c) corresponding to <i>DeviceId</i>, or
*		NULL if no match is found.
*
* @note		None.
******************************************************************************/
XCfupmc_Config *XCfupmc_LookupConfig(u16 DeviceId)
{
	extern XCfupmc_Config XCfupmc_ConfigTable[XPAR_XCFUPMC_NUM_INSTANCES];
	XCfupmc_Config *CfgPtr = NULL;
	u32 Index;

	/* Checks all the instances */
	for (Index = (u32)0x0; Index < (u32)(XPAR_XCFUPMC_NUM_INSTANCES);
								Index++) {
		if (XCfupmc_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XCfupmc_ConfigTable[Index];
			break;
		}
	}

	return (XCfupmc_Config *)CfgPtr;
}
/** @} */
