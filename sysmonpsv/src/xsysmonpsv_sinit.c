/******************************************************************************
* Copyright (C) 2016 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xsysmonpsv_sinit.c
* @addtogroup sysmonpsv_v2_3
* @{
*
* Functions in this file are the minimum required functions for the XSysMonPsv
* driver. See xsysmonpsv.h for a detailed description of the driver.
*
* @note         None.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date         Changes
* ----- -----  -------- -----------------------------------------------
* 1.0   aad    20/11/18 First release.
* 2.3   aad    07/26/21 Fixed doxygen comments.
* 3.0   cog    03/25/21 Driver Restructure
*
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "lowlevel/xsysmonpsv_hw.h"
#include "xsysmonpsv.h"
#include "xparameters.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Function Prototypes *****************************/

/************************** Variable Definitions ****************************/
extern XSysMonPsv_Config XSysMonPsv_ConfigTable[]; /**< Config Table for
                                                          sysmon device */

/*****************************************************************************/
/**
*
* This function looks for the device configuration based on the unique device
* ID. The table XSysmonPsu_ConfigTable[] contains the configuration information
* for each device in the system.
*
* @return       A pointer to the configuration table entry corresponding to the
*               given device , or NULL if no match is found.
*
* @note         None.
*
******************************************************************************/
XSysMonPsv_Config *XSysMonPsv_LookupConfig(void)
{
	XSysMonPsv_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_XSYSMONPSV_NUM_INSTANCES; Index++) {
		CfgPtr = &XSysMonPsv_ConfigTable[Index];
	}

	return CfgPtr;
}
/** @} */
