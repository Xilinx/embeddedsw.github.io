/******************************************************************************
* Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xmipi_rx_phy_sinit.c
*
* @addtogroup mipi_rx_phy Overview
* @{
*
* Look up the hardware settings using device ID. The hardware setting is inside
* the configuration table in xmipi_rx_phy_g.c, generated automatically by XPS or
* manually by the user.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver Who Date     Changes
* --- --- -------- ------------------------------------------------------------
* 1.0 pg 16/02/24 Initial release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xmipi_rx_phy.h"

/*****************************************************************************/
/**
 * Look up the hardware configuration for a device instance
 *
 * @param 	BaseAddress is the BaseAddress of the device to lookup for
 *
 * @return 	The reference to the configuration record in the configuration
 * 		table (in xmipi_rx_phy_g.c) corresponding to the BaseAddr or if
 *		not found,a NULL pointer is returned.
 *
 * @note	None
 *
 *****************************************************************************/
XMipi_Rx_Phy_Config * XMipi_Rx_Phy_LookupConfig(UINTPTR BaseAddress)
{
	extern XMipi_Rx_Phy_Config XMipi_Rx_Phy_ConfigTable[];
	XMipi_Rx_Phy_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0; XMipi_Rx_Phy_ConfigTable[Index].Name != NULL; Index++) {
		if ((XMipi_Rx_Phy_ConfigTable[Index].BaseAddr == BaseAddress) ||
				!BaseAddress) {
			CfgPtr = &XMipi_Rx_Phy_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}
/** @} */
