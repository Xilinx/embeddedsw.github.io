/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 * @file xiicps_polled_slave_example.c
 *
 * This example can run on zynqmp / versal platform evaluation board and
 * IIC controller configured slave in polled mode and Aardvark Analyzer used as
 * master.
 *
 * This example uses buffer of size 250. Please set the send buffer of the
 * Aardvark device to be continuous data from 0x00 to 0xF9.
 *
 * <pre> MODIFICATION HISTORY:
 *
 * Ver   Who Date     Changes
 * ----- --- -------- -----------------------------------------------
 * 1.00a jz  01/30/10 First release
 * 3.18  gm  07/14/23 Added SDT support.
 *
 * </pre>
 *
 ****************************************************************************/

/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifndef SDT
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#endif

/* The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x45
#define IIC_SCLK_RATE		400000

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC
 */
#define TEST_BUFFER_SIZE	250

/**************************** Type Definitions ********************************/

/************************** Function Prototypes *******************************/

#ifndef SDT
int IicPsSlavePolledExample(u16 DeviceId);
#else
int IicPsSlavePolledExample(UINTPTR BaseAddress);
#endif
/************************** Variable Definitions ******************************/

XIicPs Iic;				/* Instance of the IIC Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC. These buffers are defined as global so that they are not
 * defined on the stack.
 */
u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

/******************************************************************************/
/**
*
* Main function to call the polled slave example.
*
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/
int main(void)
{
	int Status;

	xil_printf("IIC Slave Polled Example Test \r\n");

	/*
	 * Run the Iic polled slave example , specify the Device ID that is
	 * generated in xparameters.h.
	 */
#ifndef SDT
	Status = IicPsSlavePolledExample(IIC_DEVICE_ID);
#else
	Status = IicPsSlavePolledExample(XPAR_XIICPS_0_BASEADDR);
#endif
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Slave Polled Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran IIC Slave Polled Example Test\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function does polled mode transfer in slave mode. It first sends to
* master then receives.
*
* @param	DeviceId is the Device ID of the IicPs Device and is the
*		XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*******************************************************************************/
#ifndef SDT
int IicPsSlavePolledExample(u16 DeviceId)
#else
int IicPsSlavePolledExample(UINTPTR BaseAddress)
#endif
{
	int Status;
	XIicPs_Config *Config;
	int Index;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
#ifndef SDT
	Config = XIicPs_LookupConfig(DeviceId);
#else
	Config = XIicPs_LookupConfig(BaseAddress);
#endif
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XIicPs_SetupSlave(&Iic, IIC_SLAVE_ADDR);

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified.
	 */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
		SendBuffer[Index] = (Index % TEST_BUFFER_SIZE);
		RecvBuffer[Index] = 0;
	}

	/*
	 * Send the buffer using the IIC and ignore the number of bytes sent
	 * as the return value since we are using it in interrupt mode.
	 */
	Status = XIicPs_SlaveSendPolled(&Iic, SendBuffer,
					TEST_BUFFER_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (XIicPs_BusIsBusy(&Iic)) {
		/* NOP */
	}

	Status = XIicPs_SlaveRecvPolled(&Iic, RecvBuffer,
					TEST_BUFFER_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Verify received data is correct.
	 */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index ++) {
		if (RecvBuffer[Index] != Index % TEST_BUFFER_SIZE) {
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}
