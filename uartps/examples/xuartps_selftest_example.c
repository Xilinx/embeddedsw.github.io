/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xuartps_selftest_example.c
*
* This example runs on zynqmp evaluation board (zcu102), it performs self test
* on the device by using XUartPs driver.
*
*
* @note
*
* None
*
* MODIFICATION HISTORY:
* <pre>
* Ver   Who    Date     Changes
* ----- ------ -------- -----------------------------------------------
* 1.00a drg/jz 01/13/10 First Release
* 1.03a  sg    08/14/12 Updated the example for CR 666306. Modified
*			the device ID to use the first Device Id
*			Removed the printf at the start of the main
* </pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifndef SDT
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#else
#define	XUARTPS_BASEADDRESS	XPAR_XUARTPS_0_BASEADDR
#endif

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#ifndef SDT
int UartPsSelfTestExample(u16 DeviceId);
#else
int UartPsSelfTestExample(UINTPTR BaseAddress);
#endif
/************************** Variable Definitions *****************************/

XUartPs Uart_Ps;		/* Instance of the UART Device */

/*****************************************************************************/
/**
*
* Main function to call the example.
*
*
* @return   XST_SUCCESS if successful, otherwise XST_FAILURE
*
* @note     None
*
******************************************************************************/
int main(void)
{
	int Status;

	/* Run the selftest example */
#ifndef SDT
	Status = UartPsSelfTestExample(UART_DEVICE_ID);
#else
	Status = UartPsSelfTestExample(XUARTPS_BASEADDRESS);
#endif
	if (Status != XST_SUCCESS) {
		xil_printf("UART Selftest Example Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran UART Selftest Example\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function does a minimal test on the XUartPs driver.
*
*
* @param    DeviceId is the XPAR_<UARTPS_instance>_DEVICE_ID value from
*           xparameters.h
*
* @return   XST_SUCCESS if successful, otherwise XST_FAILURE
*
* @note     None
*
****************************************************************************/
#ifndef SDT
int UartPsSelfTestExample(u16 DeviceId)
#else
int UartPsSelfTestExample(UINTPTR BaseAddress)
#endif
{
	int Status;
	XUartPs_Config *Config;

	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
#ifndef SDT
	Config = XUartPs_LookupConfig(DeviceId);
#else
	Config = XUartPs_LookupConfig(BaseAddress);
#endif
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Perform a self-test to check hardware build. */
	Status = XUartPs_SelfTest(&Uart_Ps);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
