/*******************************************************************************
* Copyright (C) 2015 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/******************************************************************************/
/**
 *
 * @file xhdmiphy1_mmcme4.c
 *
 * Contains a minimal set of functions for the XHdmiphy1 driver that allow
 * access to all of the Video PHY core's functionality. See xhdmiphy1.h for a
 * detailed description of the driver.
 *
 * @note	None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 *            dd/mm/yy
 * ----- ---- -------- -----------------------------------------------
 * 1.0   gm   10/12/18 Initial release.
 * 1.1   ssh  07/26/21 Added definitions for registers and masks
 *
 * </pre>
 *
*******************************************************************************/

/******************************* Include Files ********************************/

#include "xparameters.h"
#include "xstatus.h"
#include "xhdmiphy1.h"
#include "xhdmiphy1_i.h"
#if (XPAR_HDMIPHY1_0_TRANSCEIVER == XHDMIPHY1_GTHE4 || \
     XPAR_HDMIPHY1_0_TRANSCEIVER == XHDMIPHY1_GTYE4)

#define XHDMIPHY1_MMCM_CLKOUT0_REG1 0x08
#define XHDMIPHY1_MMCM_CLKOUT0_REG2 0x09
#define XHDMIPHY1_MMCM_CLKOUT1_REG1 0x0A
#define XHDMIPHY1_MMCM_CLKOUT1_REG2 0x0B
#define XHDMIPHY1_MMCM_CLKOUT2_REG1 0x0C
#define XHDMIPHY1_MMCM_CLKOUT2_REG2 0x0D
#define XHDMIPHY1_MMCM_CLKFBOUT_REG1 0x14
#define XHDMIPHY1_MMCM_CLKFBOUT_REG2 0x15
#define XHDMIPHY1_MMCM_DIVCLK_DIV_REG 0x16
#define XHDMIPHY1_MMCM_DRP_LOCK_REG1 0x18
#define XHDMIPHY1_MMCM_DRP_LOCK_REG2 0x19
#define XHDMIPHY1_MMCM_DRP_LOCK_REG3 0x1A
#define XHDMIPHY1_MMCM_DRP_FILTER_REG1 0x4E
#define XHDMIPHY1_MMCM_DRP_FILTER_REG2 0x4F
#define XHDMIPHY1_MMCM_PWR_REG  0x27

#define XHDMIPHY1_MMCM_WRITE_VAL 0xFFFF
#define XHDMIPHY1_PLL_WRITE_VAL  0x4401

/**************************** Function Prototypes *****************************/
static u32 XHdmiphy1_Mmcme4DividerEncoding(XHdmiphy1_MmcmDivType DivType,
		u8 Div);
static u16 XHdmiphy1_Mmcme4FilterReg1Encoding(u8 Mult);
static u16 XHdmiphy1_Mmcme4FilterReg2Encoding(u8 Mult);
static u16 XHdmiphy1_Mmcme4LockReg1Encoding(u8 Mult);
static u16 XHdmiphy1_Mmcme4LockReg2Encoding(u8 Mult);
static u16 XHdmiphy1_Mmcme4LockReg3Encoding(u8 Mult);

/************************** Constant Definitions ******************************/

/*****************************************************************************/
/**
* This function returns the DRP encoding of ClkFbOutMult optimized for:
* Phase = 0; Dutycycle = 0.5; No Fractional division
* The calculations are based on XAPP888
*
* @param	Div is the divider to be encoded
*
* @return
*		- Encoded Value for ClkReg1 [31: 0]
*       - Encoded Value for ClkReg2 [63:32]
*
* @note		None.
*
******************************************************************************/
u32 XHdmiphy1_Mmcme4DividerEncoding(XHdmiphy1_MmcmDivType DivType, u8 Div)
{
	u32 DrpEnc;
	u32 ClkReg1;
    u32 ClkReg2;
    u8 HiTime, LoTime;

    if (Div == 1) {
        ClkReg1 = 0x00001041;
        ClkReg2 = 0x00C00000;
    }
    else {
        HiTime = Div / 2;
        LoTime = Div - HiTime;

        ClkReg1 = LoTime & 0x3F;
        ClkReg1 |= (HiTime & 0x3F) << 6;
        ClkReg1 |= (DivType == XHDMIPHY1_MMCM_DIVCLK_DIVIDE) ? 0x0000 : 0x1000;

        ClkReg2 = (Div % 2) ? 0x00800000 : 0x00000000;
    }

    DrpEnc = ClkReg2 | ClkReg1;

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function returns the DRP encoding of FilterReg1 optimized for:
* Phase = 0; Dutycycle = 0.5; BW = low; No Fractional division
*
* @param	Mult is the divider to be encoded
*
* @return
*		- Encoded Value
*
* @note		None.
*
******************************************************************************/
u16 XHdmiphy1_Mmcme4FilterReg1Encoding(u8 Mult)
{
	u16 DrpEnc;

    switch (Mult) {
	case 1: case 2: case 3:
	case 4: case 5: case 6:
	case 7: case 8: case 9:
	case 10: case 11: case 12:
	case 13: case 14: case 16:
		DrpEnc = 0x0900;
		break;
    case 15:
		DrpEnc = 0x1000;
		break;
		break;
	case 17: case 19: case 20:
    case 29: case 30: case 31:
    case 38: case 39: case 40:
    case 41: case 78: case 79:
    case 80: case 81: case 82:
    case 83: case 84: case 85:
		DrpEnc = 0x9800;
		break;
    case 26: case 27: case 28:
    case 71: case 72: case 73:
    case 74: case 75: case 76:
    case 77: case 120: case 121:
    case 122: case 123: case 124:
    case 125: case 126: case 127:
    case 128:
		DrpEnc = 0x9100;
		break;
	default:
		DrpEnc = 0x9900;
		break;
	}

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function returns the DRP encoding of FilterReg1 optimized for:
* Phase = 0; Dutycycle = 0.5; BW = low; No Fractional division
*
* @param	Mult is the divider to be encoded
*
* @return
*		- Encoded Value
*
* @note		None.
*
******************************************************************************/
u16 XHdmiphy1_Mmcme4FilterReg2Encoding(u8 Mult)
{
	u16 DrpEnc;

    switch (Mult) {
	case 1: case 2:
		DrpEnc = 0x9990;
		break;
    case 3:
		DrpEnc = 0x9190;
		break;
	case 4:
		DrpEnc = 0x1190;
		break;
	case 5:
		DrpEnc = 0x8190;
		break;
    case 6: case 7:
		DrpEnc = 0x9890;
		break;
	case 8:
		DrpEnc = 0x0190;
		break;
    case 9: case 10: case 11:
	case 15: case 17: case 18:
        DrpEnc = 0x1890;
		break;
    case 12: case 13: case 14:
	case 19: case 20: case 21:
    case 22: case 23: case 24:
    case 25:
		DrpEnc = 0x8890;
		break;
    case 16:
    case 26: case 27: case 28:
    case 29: case 30: case 31:
    case 32: case 33: case 34:
    case 35: case 36: case 37:
		DrpEnc = 0x9090;
		break;
    case 38: case 39: case 40:
    case 41: case 42: case 43:
    case 44: case 45: case 46:
    case 47: case 48: case 49:
    case 50: case 51: case 52:
    case 53: case 54: case 55:
    case 56: case 57: case 58:
    case 59: case 60: case 61:
    case 62:
		DrpEnc = 0x0890;
		break;
    case 120: case 121: case 122:
    case 123: case 124: case 125:
    case 126: case 127: case 128:
		DrpEnc = 0x8090;
		break;
	default:
		DrpEnc = 0x1090;
		break;
	}

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function returns the DRP encoding of LockReg1 optimized for:
* Phase = 0; Dutycycle = 0.5; No Fractional division
* v . njm
* @param	Mult is the divider to be encoded
*
* @return
*		- Encoded Value
*
* @note		None.
*
******************************************************************************/
u16 XHdmiphy1_Mmcme4LockReg1Encoding(u8 Mult)
{
	u16 DrpEnc;

    switch (Mult) {
	case 1: case 2: case 3:
	case 4: case 5: case 6:
	case 7: case 8: case 9:
	case 10:
		DrpEnc = 0x03e8;
		break;
	case 11:
		DrpEnc = 0x0384;
		break;
	case 12:
		DrpEnc = 0x0339;
		break;
	case 13:
		DrpEnc = 0x02ee;
		break;
	case 14:
		DrpEnc = 0x02bc;
		break;
	case 15:
		DrpEnc = 0x028a;
		break;
	case 16:
		DrpEnc = 0x0271;
		break;
	case 17:
		DrpEnc = 0x023f;
		break;
	case 18:
		DrpEnc = 0x0226;
		break;
	case 19:
		DrpEnc = 0x020d;
		break;
	case 20:
		DrpEnc = 0x01f4;
		break;
	case 21:
		DrpEnc = 0x01db;
		break;
	case 22:
		DrpEnc = 0x01c2;
		break;
	case 23:
		DrpEnc = 0x01a9;
		break;
	case 24:
	case 25:
		DrpEnc = 0x0190;
		break;
	case 26:
		DrpEnc = 0x0177;
		break;
	case 27:
	case 28:
		DrpEnc = 0x015e;
		break;
	case 29:
	case 30:
		DrpEnc = 0x0145;
		break;
	case 31:
	case 32:
	case 33:
		DrpEnc = 0x012c;
		break;
	case 34:
	case 35:
	case 36:
		DrpEnc = 0x0113;
		break;
	default:
		DrpEnc = 0x00fa;
		break;
	}

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function returns the DRP encoding of LockReg2 optimized for:
* Phase = 0; Dutycycle = 0.5; No Fractional division
*
* @param	Mult is the divider to be encoded
*
* @return
*		- Encoded Value
*
* @note		None.
*
******************************************************************************/
u16 XHdmiphy1_Mmcme4LockReg2Encoding(u8 Mult)
{
	u16 DrpEnc;

    switch (Mult) {
	case 1:
	case 2:
		DrpEnc = 0x1801;
		break;
	case 3:
		DrpEnc = 0x2001;
		break;
	case 4:
		DrpEnc = 0x2c01;
		break;
	case 5:
		DrpEnc = 0x3801;
		break;
	case 6:
		DrpEnc = 0x4401;
		break;
	case 7:
		DrpEnc = 0x4c01;
		break;
	case 8:
		DrpEnc = 0x5801;
		break;
	case 9:
		DrpEnc = 0x6401;
		break;
	case 10:
		DrpEnc = 0x7001;
		break;
	default:
		DrpEnc = 0x7c01;
		break;
	}

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function returns the DRP encoding of LockReg3 optimized for:
* Phase = 0; Dutycycle = 0.5; No Fractional division
*
* @param	Mult is the divider to be encoded
*
* @return
*		- Encoded Value
*
* @note		None.
*
******************************************************************************/
u16 XHdmiphy1_Mmcme4LockReg3Encoding(u8 Mult)
{
	u16 DrpEnc;

    switch (Mult) {
	case 1:
	case 2:
		DrpEnc = 0x1be9;
		break;
	case 3:
		DrpEnc = 0x23e9;
		break;
	case 4:
		DrpEnc = 0x2fe9;
		break;
	case 5:
		DrpEnc = 0x3be9;
		break;
	case 6:
		DrpEnc = 0x47e9;
		break;
	case 7:
		DrpEnc = 0x4fe9;
		break;
	case 8:
		DrpEnc = 0x5be9;
		break;
	case 9:
		DrpEnc = 0x67e9;
		break;
	case 10:
		DrpEnc = 0x73e9;
		break;
	default:
		DrpEnc = 0x7fe9;
		break;
	}

	return DrpEnc;
}

/*****************************************************************************/
/**
* This function will write the mixed-mode clock manager (MMCM) values currently
* stored in the driver's instance structure to hardware .
*
* @param	InstancePtr is a pointer to the XHdmiphy1 core instance.
* @param	QuadId is the GT quad ID to operate on.
* @param	Dir is an indicator for TX or RX.
*
* @return
*		- XST_SUCCESS if the MMCM write was successful.
*		- XST_FAILURE otherwise, if the configuration success bit did
*		  not go low.
*
* @note		None.
*
******************************************************************************/
u32 XHdmiphy1_MmcmWriteParameters(XHdmiphy1 *InstancePtr, u8 QuadId,
							XHdmiphy1_DirectionType Dir)
{
	u8 ChId;
	u16 DrpVal;
	u32 DrpVal32;
	XHdmiphy1_Mmcm *MmcmParams;

    ChId = (Dir == XHDMIPHY1_DIR_TX) ?
						XHDMIPHY1_CHANNEL_ID_TXMMCM :
						XHDMIPHY1_CHANNEL_ID_RXMMCM;

	MmcmParams = &InstancePtr->Quads[QuadId].Mmcm[Dir];

	/* Check Parameters if has been Initialized */
	if (!MmcmParams->DivClkDivide && !MmcmParams->ClkFbOutMult &&
			!MmcmParams->ClkOut0Div && !MmcmParams->ClkOut1Div &&
			!MmcmParams->ClkOut2Div) {
		return XST_FAILURE;
	}


	/* Write Power Register Value */
	if (((Dir == XHDMIPHY1_DIR_RX) && (InstancePtr->Config.RxClkPrimitive == 0)) ||
			((Dir == XHDMIPHY1_DIR_TX) && (InstancePtr->Config.TxClkPrimitive == 0))) {
		XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId,
				XHDMIPHY1_MMCM_PWR_REG, XHDMIPHY1_MMCM_WRITE_VAL);
	} else {
		XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId,
				XHDMIPHY1_MMCM_PWR_REG, XHDMIPHY1_PLL_WRITE_VAL);
	}

	/* Write CLKFBOUT Reg1 & Reg2 Values */
	DrpVal32 = XHdmiphy1_Mmcme4DividerEncoding(XHDMIPHY1_MMCM_CLKFBOUT_MULT_F,
						MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKFBOUT_REG1,
						(u16)(DrpVal32 & XHDMIPHY1_MMCM_WRITE_VAL));
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKFBOUT_REG2,
						(u16)((DrpVal32 >> 16) & XHDMIPHY1_MMCM_WRITE_VAL));

	/* Write DIVCLK_DIVIDE Value */
	DrpVal32 = XHdmiphy1_Mmcme4DividerEncoding(XHDMIPHY1_MMCM_DIVCLK_DIVIDE,
						MmcmParams->DivClkDivide) ;
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DIVCLK_DIV_REG,
						(u16)(DrpVal32 & XHDMIPHY1_MMCM_WRITE_VAL));

	/* Write CLKOUT0 Reg1 & Reg2 Values */
	DrpVal32 = XHdmiphy1_Mmcme4DividerEncoding(XHDMIPHY1_MMCM_CLKOUT_DIVIDE,
						MmcmParams->ClkOut0Div);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT0_REG1,
						(u16)(DrpVal32 & XHDMIPHY1_MMCM_WRITE_VAL));
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT0_REG2,
						(u16)((DrpVal32 >> 16) & XHDMIPHY1_MMCM_WRITE_VAL));

	/* Write CLKOUT1 Reg1 & Reg2 Values */
	DrpVal32 = XHdmiphy1_Mmcme4DividerEncoding(XHDMIPHY1_MMCM_CLKOUT_DIVIDE,
						MmcmParams->ClkOut1Div);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT1_REG1,
						(u16)(DrpVal32 & XHDMIPHY1_MMCM_WRITE_VAL));
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT1_REG2,
						(u16)((DrpVal32 >> 16) & XHDMIPHY1_MMCM_WRITE_VAL));

	/* Write CLKOUT2 Reg1 & Reg2 Values */
	DrpVal32 = XHdmiphy1_Mmcme4DividerEncoding(XHDMIPHY1_MMCM_CLKOUT_DIVIDE,
						MmcmParams->ClkOut2Div);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT2_REG1,
						(u16)(DrpVal32 & XHDMIPHY1_MMCM_WRITE_VAL));
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_CLKOUT2_REG2,
						(u16)((DrpVal32 >> 16) & XHDMIPHY1_MMCM_WRITE_VAL));
	/* Write Lock Reg1 Value */
	DrpVal = XHdmiphy1_Mmcme4LockReg1Encoding(MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DRP_LOCK_REG1, DrpVal);

	/* Write Lock Reg2 Value */
	DrpVal = XHdmiphy1_Mmcme4LockReg2Encoding(MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DRP_LOCK_REG2, DrpVal);

	/* Write Lock Reg3 Value */
	DrpVal = XHdmiphy1_Mmcme4LockReg3Encoding(MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DRP_LOCK_REG3, DrpVal);

	/* Write Filter Reg1 Value */
	DrpVal = XHdmiphy1_Mmcme4FilterReg1Encoding(MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DRP_FILTER_REG1, DrpVal);

	/* Write Filter Reg2 Value */
	DrpVal = XHdmiphy1_Mmcme4FilterReg2Encoding(MmcmParams->ClkFbOutMult);
	XHdmiphy1_DrpWr(InstancePtr, QuadId, ChId, XHDMIPHY1_MMCM_DRP_FILTER_REG2, DrpVal);

	return XST_SUCCESS;
}

#endif
