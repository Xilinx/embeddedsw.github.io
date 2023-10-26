/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/**
*
* @file xaxiethernet_sinit.c
* @addtogroup axiethernet Overview
* @{
*
* This file contains static initialization functionality for Axi Ethernet driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a asa  6/30/10  First release
* 5.6   ms   08/07/17 Fixed compilation warning.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xaxiethernet.h"
#ifndef SDT
#include "xparameters.h"
#endif

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* XAxiEthernet_LookupConfig returns a reference to an XAxiEthernet_Config
* structure based on an unique device id, <i>DeviceId</i>. The return value
* will refer to an entry in the device configuration table defined in the
* xaxiethernet_g.c file.
*
* @param	DeviceId is the unique device ID of the device for the lookup
*		operation.
*
* @return
*		- Returns a reference to a config record in the
*		  configuration table (in xaxiethernet_g.c) corresponding to
*		  <i>DeviceId</i>, or NULL
*		- NULL if no match is found.
*
******************************************************************************/
#ifndef SDT
XAxiEthernet_Config *XAxiEthernet_LookupConfig(u16 DeviceId)
{
	extern XAxiEthernet_Config XAxiEthernet_ConfigTable[];
	XAxiEthernet_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < XPAR_XAXIETHERNET_NUM_INSTANCES; Index++) {
		if (XAxiEthernet_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XAxiEthernet_ConfigTable[Index];
			break;
		}
	}

	return (CfgPtr);
}
#else
XAxiEthernet_Config *XAxiEthernet_LookupConfig(UINTPTR BaseAddress)
{
	extern XAxiEthernet_Config XAxiEthernet_ConfigTable[];
	XAxiEthernet_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0x0; XAxiEthernet_ConfigTable[Index].Name != NULL; Index++) {
		if ((XAxiEthernet_ConfigTable[Index].BaseAddress == BaseAddress) ||
		     !BaseAddress) {
			CfgPtr = &XAxiEthernet_ConfigTable[Index];
			break;
		}
	}


	return (CfgPtr);
}
#endif
/** @} */
