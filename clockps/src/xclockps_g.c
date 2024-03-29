/******************************************************************************
* Copyright (C) 2018 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xclockps_g.c
* @addtogroup clockps Overview
* @{
*
* This file contains a table that specifies the configuration of the clocking
* in the system. Each device should have an entry in the table.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ---------------------------------------------
* 1.00  cjp    02/09/18 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xclockps.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Prototypes ******************************/
/**
 * This table contains configuration information for clocking device in
 * the system.
 */
XClockPs_Config XClockPs_ConfigTable[XPAR_XCLOCKPS_NUM_INSTANCES] = {
	{
		(u16)XPAR_XCLOCKPS_DEVICE_ID,
	}
};

/** @} */
