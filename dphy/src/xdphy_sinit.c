/******************************************************************************
*
* Copyright (C) 2015 - 2016 Xilinx, Inc. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xdphy_sinit.c
*
* @addtogroup dphy_v1_0
* @{
*
* Look up the hardware settings using device ID. The hardware setting is inside
* the configuration table in xdphy_g.c, generated automatically by XPS or
* manually by the user.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver Who Date     Changes
* --- --- -------- ------------------------------------------------------------
* 1.0 vsa 07/09/15 Initial release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xdphy.h"

/*****************************************************************************/
/**
 * Look up the hardware configuration for a device instance
 *
 * @param 	DeviceId is the unique device ID of the device to lookup for
 *
 * @return 	The reference to the configuration record in the configuration
 * 		table (in xdphy_g.c) corresponding to the Device ID or if
 *		not found,a NULL pointer is returned.
 *
 * @note	None
 *
 *****************************************************************************/
XDphy_Config * XDphy_LookupConfig(u32 DeviceId)
{
	extern XDphy_Config XDphy_ConfigTable[];
	XDphy_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0; Index < XPAR_XDPHY_NUM_INSTANCES; Index++) {
		if (XDphy_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XDphy_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}
/** @} */
