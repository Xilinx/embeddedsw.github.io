/***************************************************************************************************
* Copyright (c) 2023 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
***************************************************************************************************/

/*************************************************************************************************/
/**
 * @file  xtrngpsx_hrng_example.c
 *
 * This example demonstrates use of instantiate, reseed, generate functionalities while generating
 * random number using HRNG mode and runs DRBG,Health tests for all the instances
 * i.e XTrngpsx_PreOperationalSelfTests.
 *
 * @note
 *
 * None.
 *
 * MODIFICATION HISTORY:
 *
 *<pre>
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 * 1.00  kpt  01/10/23 First release
 *
 *</pre>
 **************************************************************************************************/

/*************************************** Include Files *******************************************/

#include "xparameters.h"
#include "xtrngpsx.h"
#include "xil_util.h"

/************************************ Constant Definitions ***************************************/

/************************************** Type Definitions *****************************************/

/*************************** Macros (Inline Functions) Definitions *******************************/

/************************************ Function Prototypes ****************************************/

static int Trngpsx_Example();
static void Trngpsx_PrintBytes(u8 *Src, u32 Size);

/************************************ Variable Definitions ***************************************/

XTrngpsx_Instance Trngpsx; /* Instance of TRNGPSX */
u8 RandBuf[XTRNGPSX_SEC_STRENGTH_IN_BYTES];

const u8 PersStr[XTRNGPSX_PERS_STRING_LEN_IN_BYTES] = {
		0xB2U, 0x80U, 0x7EU, 0x4CU, 0xD0U, 0xE4U, 0xE2U, 0xA9U,
		0x2FU, 0x1FU, 0x5DU, 0xC1U, 0xA2U, 0x1FU, 0x40U, 0xFCU,
		0x1FU, 0x24U, 0x5DU, 0x42U, 0x61U, 0x80U, 0xE6U, 0xE9U,
		0x71U, 0x05U, 0x17U, 0x5BU, 0xAFU, 0x70U, 0x30U, 0x18U,
		0xBCU, 0x23U, 0x18U, 0x15U, 0xCBU, 0xB8U, 0xA6U, 0x3EU,
		0x83U, 0xB8U, 0x4AU, 0xFEU, 0x38U, 0xFCU, 0x25U, 0x87U
};

/*************************************************************************************************/
/**
 * @brief
 * Main function to call the Trngpsx driver example.
 *
 * @return
 *		- XST_SUCCESS if example ran successfully.
 *		- XST_FAILURE if unsuccessful.
 *
 **************************************************************************************************/
int main(void)
{
	int Status;

	xil_printf("****** TRNGPSX example *******\n\r");
	Status = Trngpsx_Example();
	if (Status != XST_SUCCESS) {
		xil_printf("TRNGPSX example failed\n\r");
		return XST_FAILURE;
	}
	xil_printf("Successfully ran TRNGPSX HRNG example\n\r");

	return XST_SUCCESS;
}

/*************************************************************************************************/
/**
 * @brief
 * This function tests the functioning of the TRNG in HRNG mode.
 *
 * @return
 *		- XST_SUCCESS if example runs successfully.
 *		- XST_FAILURE otherwise.
 *
 ************************************************************************************************/
int Trngpsx_Example()
{
	int Status = XST_SUCCESS;
	XTrngpsx_Config *Config;
	XTrngpsx_UserConfig UsrCfg = {
			.Mode = XTRNGPSX_HRNG_MODE,
			.SeedLife = XTRNGPSX_USER_CFG_SEED_LIFE,
			.DFLength = XTRNGPSX_USER_CFG_DF_LENGTH,
			.AdaptPropTestCutoff = XTRNGPSX_USER_CFG_ADAPT_TEST_CUTOFF,
			.RepCountTestCutoff = XTRNGPSX_USER_CFG_REP_TEST_CUTOFF,
	};

	for (u32 DeviceId = 0U; DeviceId < XPAR_XTRNGPSX_NUM_INSTANCES; DeviceId++) {
	   /*
		* Initialize the TRNGPSX driver so that it's ready to use look up
		* configuration in the config table, then initialize it.
		*/
		Config = XTrngpsx_LookupConfig(DeviceId);
		if (NULL == Config) {
			xil_printf("LookupConfig Failed \n\r");
			goto END;
		}

		/* Initialize the TRNGPSX driver so that it is ready to use. */
		Status = XTrngpsx_CfgInitialize(&Trngpsx, Config, Config->BaseAddress);
		if (Status != XST_SUCCESS) {
			xil_printf("CfgInitialize Failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		xil_printf("\n\r TRNGPSX example with instance:%d \n\r ", DeviceId);
		Status = XTrngpsx_PreOperationalSelfTests(&Trngpsx);
		if (Status != XST_SUCCESS) {
			xil_printf("KAT Failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		/* Instantiate to complete initialization and for (initial) reseeding */
		Status = XTrngpsx_Instantiate(&Trngpsx, NULL, 0U, PersStr, &UsrCfg);
		if (Status != XST_SUCCESS) {
			xil_printf("Instantiate failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		/* Reseed with DFLenMul of 7 */
		Status = XTrngpsx_Reseed(&Trngpsx, NULL, XTRNGPSX_USER_CFG_DF_LENGTH);
		if (Status != XST_SUCCESS) {
			xil_printf("Reseed failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		/* Invoke Generate twice and print, RandBuf contains random data from last call */
		Status = XTrngpsx_Generate(&Trngpsx, RandBuf, sizeof(RandBuf), FALSE);
		if (Status != XST_SUCCESS) {
			xil_printf("Generate Failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		xil_printf("Generate 1 Random data:\n\r");
		Trngpsx_PrintBytes(RandBuf, sizeof(RandBuf));

		Status = XTrngpsx_Generate(&Trngpsx, RandBuf, sizeof(RandBuf), FALSE);
		if (Status != XST_SUCCESS) {
			xil_printf("Generate Failed, Status: 0x%08x\n\r", Status);
			goto END;
		}

		xil_printf("Generate 2 Random data:\n\r");
		Trngpsx_PrintBytes(RandBuf, sizeof(RandBuf));

		Status = XTrngpsx_Uninstantiate(&Trngpsx);
		if (Status != XST_SUCCESS) {
			xil_printf("Uninstantiate Failed \n\r");
			goto END;
		}
	}

		Status = XST_SUCCESS;
END:
	return Status;
}

/*************************************************************************************************/
/**
 * @brief
 * This function prints specified number of bytes from an address.
 *
 * @param	src is start address from where to print.
 *
 * @param	size is size of buffer pointed by src.
 *
 * @return	None.
 *
 ************************************************************************************************/
static void Trngpsx_PrintBytes(u8 *Src, u32 Size)
{
	u32 Index;

	for (Index = 0; Index < Size; Index++) {
		if (Index % 16 == 0) {
			xil_printf("\n\r");
		}
		xil_printf("0x%02X, ", Src[Index]);
	}
	xil_printf("\n\r");
}
