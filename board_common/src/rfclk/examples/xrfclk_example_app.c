/******************************************************************************
* Copyright (C) 2021-2022 Xilinx, Inc. All rights reserved.
* Copyright (C) 2022-2025 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xrfclk_examples_app.c
*
* Contains the examples which run most of the APIs.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ---    -------- -----------------------------------------------
* 1.0   dc     07/21/19 Initial version
* 1.1   dc     11/21/19 Remove xil dependencies from linux build
*       dc     12/05/19 adjust LMX and LMK configs to a rftool needs
* 1.5   dc     18/01/21 pass GPIO Mux base address as parameter
* 1.6   dc     19/01/24 Correct linux gpio ID
* 2.0   dc     24/03/25 Update BM for SDT
*
* </pre>
*
******************************************************************************/

#include "xrfclk.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __BAREMETAL__
#include "xstatus.h"
#include "xil_printf.h"
#else
#include <errno.h>
#include <sys/fcntl.h>
#endif

static u32 data[256];
static int resetAll()
{
	int ret = EXIT_FAILURE;
	printf("\nReset LMK");
	if (XST_FAILURE == XRFClk_ResetChip(RFCLK_LMK)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMK)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nReset LMX2594_1");
	if (XST_FAILURE == XRFClk_ResetChip(RFCLK_LMX2594_1)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_1)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nReset LMX2594_2");
	if (XST_FAILURE == XRFClk_ResetChip(RFCLK_LMX2594_2)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_2)");
		goto ret_jump;
	} else
		printf("      Pass");
#ifdef XPS_BOARD_ZCU111
	printf("\nReset LMX2594_3");
	if (XST_FAILURE == XRFClk_ResetChip(RFCLK_LMX2594_3)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_3)");
		goto ret_jump;
	} else
		printf("      Pass");
#endif
	ret = EXIT_SUCCESS;
ret_jump:
	return ret;
}

static int getConfigAll()
{
	int i;
	int ret = EXIT_FAILURE;

	printf("\nGet config from ID on LMX2594_1");
	if (XST_FAILURE == XRFClk_GetConfigFromOneChip(RFCLK_LMX2594_1, data)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_1)");
		goto ret_jump;
	} else {
		printf("      Pass");
		printf("\nLMX2594 config data are:\n");
		for (i = 0; i < LMX2594_COUNT; i++)
			printf("%x,", data[i]);
	}
	printf("\nGet config from ID on LMX2594_2");
	if (XST_FAILURE == XRFClk_GetConfigFromOneChip(RFCLK_LMX2594_2, data)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_2)");
		goto ret_jump;
	} else {
		printf("      Pass");
		printf("\nLMX2594 config data are:\n");
		for (i = 0; i < LMX2594_COUNT; i++)
			printf("%x,", data[i]);
	}
#ifdef XPS_BOARD_ZCU111
	printf("\nGet config from ID on LMX2594_3");
	if (XST_FAILURE == XRFClk_GetConfigFromOneChip(RFCLK_LMX2594_3, data)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_3)");
		goto ret_jump;
	} else {
		printf("      Pass");
		printf("\nLMX2594 config data are:\n");
		for (i = 0; i < LMX2594_COUNT; i++)
			printf("%x,", data[i]);
	}
#else
	printf("\nGet config from ID on LMK");
	if (XST_FAILURE == XRFClk_GetConfigFromOneChip(RFCLK_LMK, data)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMK)");
		goto ret_jump;
	} else {
		printf("      Pass");
		printf("\nLMK config data are:\n");
		for (i = 0; i < LMK_COUNT; i++)
			printf("%x,", data[i]);
	}
#endif
	ret = EXIT_SUCCESS;
ret_jump:
	return ret;
}

int main()
{
	int ret = EXIT_FAILURE;
	u32 d;

	printf("\n----------- START ------------\n");
#if defined XPS_BOARD_ZCU111
	XRFClk_Init();
#elif defined __BAREMETAL__
	/* The base address is defined in xparameters.h */
#ifndef SDT
	XRFClk_Init(XPAR_PS_SUBSYSTEM_AXI_GPIO_SPI_MUX_DEVICE_ID);
#else
	XRFClk_Init(XPAR_PS_SUBSYSTEM_AXI_GPIO_SPI_MUX_BASEADDR);
#endif
#else
	/* The argument in XRFClk_Init is a gpioID.
	   Note: gpioID must be detected manualy, first find a gpio base
	         address in device tree (currently is 0xa0205000), second
	         check which gpio device has this address alocated, eg.:
	             xilinx-zcu216:~$ cat /sys/class/gpio/gpiochip481/label
	             a0205000.gpio
	        the gpio ID value is 481.
	     */
	XRFClk_Init(481);
#endif

	/* Reset */
	if (resetAll() == EXIT_FAILURE)
		goto ret_jump;

	/* Write/Read dummy value to LMX2594 */
	d = 0x20112;
	printf("\nWrite dummy data to register in LMX2594_1");
	if (XST_FAILURE == XRFClk_WriteReg(RFCLK_LMX2594_1, d)) {
		printf("\nFailure in XRFClk_WriteReg(RFCLK_LMX2594_1)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nRead and validate register value in LMX2594_1");
	if (XST_FAILURE == XRFClk_ReadReg(RFCLK_LMX2594_1, &d)) {
		printf("\nFailure in XRFClk_ReadReg(RFCLK_LMX2594_1)");
		goto ret_jump;
	}
	printf("\nread value = %x      Pass", d);
	d = 0x20212;
	/* Write/Read dummy value to LMX2594 */
	printf("\nWrite dummy data to register in LMX2594_1");
	if (XST_FAILURE == XRFClk_WriteReg(RFCLK_LMX2594_2, d)) {
		printf("\nFailure in XRFClk_WriteReg(RFCLK_LMX2594_2)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nRead and validate register value in LMX2594_1");
	if (XST_FAILURE == XRFClk_ReadReg(RFCLK_LMX2594_2, &d)) {
		printf("\nFailure in XRFClk_ReadReg(RFCLK_LMX2594_2)");
		goto ret_jump;
	}
	printf("\nread value = %x      Pass", d);
#ifdef XPS_BOARD_ZCU111
	d = 0x20312;
	/* Write/Read dummy value to LMX2594 */
	printf("\nWrite dummy data to register in LMX2594_3");
	if (XST_FAILURE == XRFClk_WriteReg(RFCLK_LMX2594_3, d)) {
		printf("\nFailure in XRFClk_WriteReg(RFCLK_LMX2594_3)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nRead and validate register value in LMX2594_3");
	if (XST_FAILURE == XRFClk_ReadReg(RFCLK_LMX2594_3, &d)) {
		printf("\nFailure in XRFClk_ReadReg(RFCLK_LMX2594_3)");
		goto ret_jump;
	}
	printf("\nread value = %x      Pass", d);
#endif

	/* Set config with ID */
	printf("\nSet config from ID on LMK");
	if (XST_FAILURE ==
	    XRFClk_SetConfigOnOneChipFromConfigId(RFCLK_LMK, 0)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMK)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nSet config from ID on LMX2594_1");
	if (XST_FAILURE ==
	    XRFClk_SetConfigOnOneChipFromConfigId(RFCLK_LMX2594_1, 0)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_1)");
		goto ret_jump;
	} else
		printf("      Pass");
	printf("\nSet config from ID on LMX2594_2");
	if (XST_FAILURE ==
	    XRFClk_SetConfigOnOneChipFromConfigId(RFCLK_LMX2594_2, 1)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_2)");
		goto ret_jump;
	} else
		printf("      Pass");
#ifdef XPS_BOARD_ZCU111
	printf("\nSet config from ID on LMX2594_3");
	if (XST_FAILURE ==
	    XRFClk_SetConfigOnOneChipFromConfigId(RFCLK_LMX2594_3, 2)) {
		printf("\nFailure in XRFClk_ResetChip(RFCLK_LMX2594_3)");
		goto ret_jump;
	} else
		printf("      Pass");
#endif

	/* Get config from chip */
	if (getConfigAll() == EXIT_FAILURE)
		goto ret_jump;

	/* wait a little */
	sleep(1);
	/* Reset */
	if (resetAll() == EXIT_FAILURE)
		goto ret_jump;

	/* Set config on all chips */
	printf("\nSet config on all RF chips");
#ifdef XPS_BOARD_ZCU111
	if (XST_FAILURE == XRFClk_SetConfigOnAllChipsFromConfigId(1, 2, 3, 4)) {
#else
	if (XST_FAILURE == XRFClk_SetConfigOnAllChipsFromConfigId(1, 2, 3)) {
#endif
		printf("\nFailure in XRFClk_SetConfigOnAllChipsFromConfigId()");
		goto ret_jump;
	} else
		printf("      Pass");

	/* Get config from chip */
	if (getConfigAll() == EXIT_FAILURE)
		goto ret_jump;

	printf("\n----------- STOP ------------\n");
	XRFClk_Close();

	ret = EXIT_SUCCESS;
ret_jump:
	return ret;
}
