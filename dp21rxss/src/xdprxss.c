/******************************************************************************
* Copyright (C) 2017 - 2023 Xilinx, Inc. All rights reserved.
* Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xdprxss.c
* @addtogroup dprxss Overview
* @{
*
* This is the main file for Xilinx DisplayPort Receiver Subsystem driver.
* This file contains a minimal set of functions for the XDpRxSs driver that
* allow access to all of the DisplayPort Receiver Subsystem core's
* functionality. Please see xdprxss.h for more details of the driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver  Who Date     Changes
* ---- --- -------- ----------------------------------------------------------
* 1.00 sha 05/18/15 Initial release.
* 2.00 sha 10/05/15 Added HDCP and Timer Counter support.
*                   Protected HDCP under macro number of instances.
* 2.00 sha 10/15/15 Generate a HPD interrupt whenever RX cable
*                   disconnect/unplug interrupt is detected.
* 2.00 sha 11/06/15 Modified the order of execution in TP1 callback as DP159
*                   config for TP1 and then link bandwidth callback.
* 3.0  sha 02/05/16 Added support for multiple subsystems in a design.
* 3.0  sha 02/19/16 Added function: XDpRxSs_DownstreamReady.
* 3.1  als 08/08/16 Synchronize with new HDCP APIs.
* 3.1  als 08/08/16 Added HDCP timeout functionality.
* 3.1  aad 09/06/16 Updates to support 64-bit base addresses.
* 3.1  aad 10/17/16 Updated timer initialization
* 4.0  aad 11/15/16 Modified to use DP159 from dprxss driver
* 4.0  aad 01/20/17 Added HDCP FIFO reset for correct initialization of the
*		    FIFO
* 4.0  tu  05/30/17 Moved timer functions under XPAR_XHDCP_NUM_INSTANCES
*		    to solve compiler warnings
* 4.1  tu  09/08/17 Set Driver side three interrupt handler callback in
*                   XDpRxSs_CfgInitialize function
* 5.0  yas 01/28/18 Added support for DP 1.4.
* 5.0  jb  02/19/19 Added support for HDCP22
* 6.0  rg  11/19/19 Added support to use PS I2C instance too.
* 6.0  jb  02/14/20 The DP Rx subsystems assumes that the HDCP configuration is
* 		    same for all the instances in multiple subsystems in the
* 		    design. This driver wont support for different configuration
* 		    of the subsystems.
* 6.1  rg  08/13/20 Added below list APIs related to Adaptive-Sync feature.
* 					XDpRxSs_SetAdaptiveSyncCaps
* 					XDpRxSs_MaskAdaptiveIntr
* 					XDpRxSs_UnMaskAdaptiveIntr
* 					XDpRxSs_GetVblank
* 					XDpRxSs_GetVtotal
* 6.1  rg  09/23/20 Added below list of APIs related to color encoding
*                   parameters,
*                   XDpRxss_GetBpc
*                   XDpRxss_GetColorComponent
*                   XDpRxss_GetColorimetry
*                   XDpRxss_GetDynamicRange
* 8.0  jb  01/12/22 Added clk_wizard configuration for rx_dec_clk which is
*                   required for 8b10b logic.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xdprxss.h"
#include "xdprxss_mcdp6000.h"
#include "string.h"
#include "xdebug.h"
#include "sleep.h"

/************************** Constant Definitions *****************************/

extern u32 MCDP6000_IC_Rev;

/***************** Macros (Inline Functions) Definitions *********************/

#define EDID_IIC_ADDRESS	0x50

/* Clock Wizard registers */
#define XCLK_WIZ_SWRST_OFFSET	0x00000000
#define XCLK_WIZ_SWRST_VAL	0x0A
#define XCLK_WIZ_STATUS_OFFSET	0x00000004
#define XCLK_WIZ_ISR_OFFSET	0x0000000C
#define XCLK_WIZ_IER_OFFSET	0x00000010
#define XCLK_WIZ_RECONFIG_OFFSET	0x00000014
#define XCLK_WIZ_RECONFIG_VAL	0x03
#define XCLK_WIZ_REG1_OFFSET	0x00000330
#define XCLK_WIZ_REG2_OFFSET	0x00000334
#define XCLK_WIZ_REG3_OFFSET	0x00000338
#define XCLK_WIZ_REG4_OFFSET	0x0000033C
#define XCLK_WIZ_REG12_OFFSET	0x00000380
#define XCLK_WIZ_REG13_OFFSET	0x00000384
#define XCLK_WIZ_REG11_OFFSET	0x00000378
#define XCLK_WIZ_REG11_VAL	0x2E
#define XCLK_WIZ_REG14_OFFSET	0x00000398
#define XCLK_WIZ_REG14_VAL	0xE80
#define XCLK_WIZ_REG15_OFFSET	0x0000039C
#define XCLK_WIZ_REG15_VAL	0x4271
#define XCLK_WIZ_REG16_OFFSET	0x000003A0
#define XCLK_WIZ_REG16_VAL	0x43E9
#define XCLK_WIZ_REG17_OFFSET	0x000003A8
#define XCLK_WIZ_REG17_VAL	0x1C
#define XCLK_WIZ_REG19_OFFSET	0x000003CC
#define XCLK_WIZ_REG25_OFFSET	0x000003F0
#define XCLK_WIZ_REG26_OFFSET	0x000003FC
#define XCLK_WIZ_REG26_VAL	0x01

/* Lock */
#define XCLK_WIZ_LOCK			1
#define XCLK_WIZ_REG3_PREDIV2		(1 << 11)
#define XCLK_WIZ_REG3_USED		(1 << 12)
#define XCLK_WIZ_REG3_MX		(1 << 9)
#define XCLK_WIZ_REG1_PREDIV2		(1 << 12)
/* FBout enable */
#define XCLK_WIZ_REG1_EN		(1 << 9)
#define XCLK_WIZ_REG1_MX		(1 << 10)
#define XCLK_WIZ_RECONFIG_LOAD		1
#define XCLK_WIZ_RECONFIG_SADDR		2
#define XCLK_WIZ_REG1_EDGE_MASK		(1 << 8)

#define XCLK_WIZ_CLKOUT0_PREDIV2_SHIFT	11
#define XCLK_WIZ_CLKOUT0_MX_SHIFT	9
#define XCLK_WIZ_CLKOUT0_P5EN_SHIFT	13
#define XCLK_WIZ_CLKOUT0_P5FEDGE_SHIFT	15
#define XCLK_WIZ_REG12_EDGE_SHIFT	10

#define M_VAL_405	28
#define M_VAL_270	44
#define M_VAL_135	88
#define M_VAL_81	148
#define D_VAL_ALL	5

#define XCLK_WIZ_STATUS_RETRY	10000
#define XCLK_WIZ_STATUS_WAIT	100

/**************************** Type Definitions *******************************/

/* Subsystem sub-core's structure includes instances of each sub-core */
typedef struct {
	XDp DpInst;
#ifdef XPAR_XIIC_NUM_INSTANCES
	XIic IicInst;
#endif
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	XHdcp1x Hdcp1xInst;
#endif
#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
			(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
	XTmrCtr TmrCtrInst;
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	XHdcp22_Rx_Dp Hdcp22Inst;
#endif
} XDpRxSs_SubCores;

/************************** Function Prototypes ******************************/
#ifndef SDT
static void DpRxSs_GetIncludedSubCores(XDpRxSs *InstancePtr);
#else
static void DpRxSs_GetIncludedSubCores(XDpRxSs *InstancePtr,
                                 UINTPTR BaseAddress);
#endif
static void DpRxSs_PopulateDpRxPorts(XDpRxSs *InstancePtr);
static void StubTp1Callback(void *InstancePtr);
static void StubTp2Callback(void *InstancePtr);
static void StubUnplugCallback(void *InstancePtr);
static void StubAccessLaneSetCallback(void *InstancePtr);

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
static int DpRxSs_HdcpStartTimer(void *InstancePtr, u16 TimeoutInMs);
static int DpRxSs_HdcpStopTimer(void *InstancePtr);
static int DpRxSs_HdcpBusyDelay(void *InstancePtr, u16 DelayInMs);
static void DpRxSs_TimerCallback(void *InstancePtr, u8 TmrCtrNumber);
static u32 DpRxSs_ConvertUsToTicks(u32 TimeoutInUs, u32 ClkFreq);
#endif

#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
			(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
static void DpRxSs_TimeOutCallback(void *InstancePtr, u8 TmrCtrNumber);
#endif

#if ((XPAR_XHDCP_NUM_INSTANCES > 0) || (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0))
static int XDpRxSs_HdcpReset(XDpRxSs *InstancePtr);
#endif

static void XDpRxSs_Set_Dec_Clk(XDpRxSs *InstancePtr);

/************************** Variable Definitions *****************************/

/* A generic EDID structure. */
u8 GenEdid[128] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x61, 0x2c, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12,
	0x01, 0x18, 0x01, 0x04, 0xa0, 0x2f, 0x1e, 0x78,
	0x00, 0xee, 0x95, 0xa3, 0x54, 0x4c, 0x99, 0x26,
	0x0f, 0x50, 0x54, 0x21, 0x08, 0x00, 0x71, 0x4f,
	0x81, 0x80, 0xb3, 0x00, 0xd1, 0xc0, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a,
	0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c,
	0x45, 0x00, 0x40, 0x84, 0x63, 0x00, 0x00, 0x1e,
	0x00, 0x00, 0x00, 0xff, 0x00, 0x58, 0x49, 0x4c,
	0x44, 0x50, 0x53, 0x49, 0x4e, 0x4b, 0x0a, 0x20,
	0x20, 0x20, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x58,
	0x49, 0x4c, 0x20, 0x44, 0x50, 0x0a, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd,
	0x00, 0x38, 0x3c, 0x1e, 0x53, 0x10, 0x00, 0x0a,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x39
};

/* A generic DPCD structure. */
u8 GenDpcd[] = {
	0x12, 0x0a, 0x84, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02
};

/* DisplayPort RX subcores instance */
XDpRxSs_SubCores DpRxSsSubCores[XPAR_XDPRXSS_NUM_INSTANCES];

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
*
* This function initializes the DisplayPort Receiver Subsystem core. This
* function must be called prior to using the core. Initialization of the core
* includes setting up the instance data and ensuring the hardware is in a
* quiescent state.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	CfgPtr points to the configuration structure associated with
*		the DisplayPort RX Subsystem core.
* @param	EffectiveAddr is the base address of the device. If address
*		translation is being used, then this parameter must reflect the
*		virtual base address. Otherwise, the physical address should be
*		used.
*
* @return
*		- XST_DEVICE_NOT_FOUND if sub-core not found.
*		- XST_FAILURE if sub-core initialization failed.
*		- XST_SUCCESS if XDpRxSs_CfgInitialize successful.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_CfgInitialize(XDpRxSs *InstancePtr, XDpRxSs_Config *CfgPtr,
				UINTPTR EffectiveAddr)
{
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	XHdcp1x_Config *Hdcp1xConfig;
#endif
#ifdef XPAR_XIIC_NUM_INSTANCES
	XIic_Config IicConfig;
#endif
	XDp_Config DpConfig;
	u32 Status;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(CfgPtr != NULL);
	Xil_AssertNonvoid(EffectiveAddr != (u32)0x0);

	/* Setup the instance */
	(void)memcpy((void *)&(InstancePtr->Config), (const void *)CfgPtr,
			sizeof(XDpRxSs_Config));

	InstancePtr->Config.BaseAddress = EffectiveAddr;

#ifndef SDT
	DpRxSs_GetIncludedSubCores(InstancePtr);
#else
	/* Get included sub-cores in the DisplayPort RX Subsystem */
	DpRxSs_GetIncludedSubCores(InstancePtr, InstancePtr->Config.BaseAddress);
#endif

	if (InstancePtr->Config.IncludeClkWiz)
#ifndef SDT
		InstancePtr->clk_wiz_abs_addr =
			InstancePtr->Config.BaseAddress +
			InstancePtr->Config.ClkWizSubCore.ClkWizConfig.AbsAddr;
#else
		InstancePtr->clk_wiz_abs_addr =
			InstancePtr->Config.BaseAddress +
			InstancePtr->Config.ClkWizSubCore.ClkWizConfig.BaseAddress;
#endif

	/* Check for IIC availability */
#ifdef XPAR_XIIC_NUM_INSTANCES
	if (InstancePtr->Config.IncludeAxiIic && InstancePtr->IicPtr) {
		xdbg_printf((XDBG_DEBUG_GENERAL),"SS INFO: Initializing "
			"IIC IP\n\r");

		/* Calculate absolute base address of IIC sub-core */
		InstancePtr->Config.IicSubCore.IicConfig.BaseAddress +=
					InstancePtr->Config.BaseAddress;

		/* Copy IIC config into local IIC config */
		(void)memcpy((void *)&(IicConfig),
			(const void *)&CfgPtr->IicSubCore.IicConfig,
				sizeof(IicConfig));

		/* IIC config initialize */
		IicConfig.BaseAddress += InstancePtr->Config.BaseAddress;
		Status = XIic_CfgInitialize(InstancePtr->IicPtr, &IicConfig,
				IicConfig.BaseAddress);
		if ((Status != XST_SUCCESS) && (Status !=
						XST_DEVICE_IS_STARTED)) {
			xdbg_printf(XDBG_DEBUG_GENERAL,"SS ERR:: IIC "
				"initialization failed!\n\r");
			return XST_FAILURE;
		}

		/* IIC initialization for dynamic functionality */
		Status = XIic_DynamicInitialize(InstancePtr->IicPtr);
		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_GENERAL,"SS ERR:: IIC "
				"dynamic initialization failed!\n\r");
			return XST_FAILURE;
		}
	}
#endif
	/* Check for DisplayPort availability */
	if (InstancePtr->DpPtr) {
		xdbg_printf((XDBG_DEBUG_GENERAL),"SS INFO: Initializing "
			"DisplayPort Receiver IP\n\r");

		/* Assign number of streams to one when MST is not enabled */
		if (InstancePtr->Config.MstSupport) {
			InstancePtr->UsrOpt.NumOfStreams =
					InstancePtr->Config.NumMstStreams;
		}
		else {
			InstancePtr->Config.DpSubCore.DpConfig.NumMstStreams =
				1;
			InstancePtr->UsrOpt.NumOfStreams = 1;
			InstancePtr->Config.NumMstStreams = 1;
		}

		/* Calculate absolute base address of DP sub-core */
		InstancePtr->Config.DpSubCore.DpConfig.BaseAddr +=
					InstancePtr->Config.BaseAddress;

		/* Copy DP config into local DP config */
		(void)memcpy((void *)&(DpConfig),
			(const void *)&CfgPtr->DpSubCore.DpConfig,
				sizeof(XDp_Config));

		/* DisplayPort config initialize */
		DpConfig.BaseAddr += InstancePtr->Config.BaseAddress;
		XDp_CfgInitialize(InstancePtr->DpPtr, &DpConfig,
				DpConfig.BaseAddr);

		/* Set maximum link rate for the first time */
		XDp_RxSetLinkRate(InstancePtr->DpPtr,
				InstancePtr->DpPtr->Config.MaxLinkRate);

		/* Set maximum lane count for the first time */
		XDp_RxSetLaneCount(InstancePtr->DpPtr,
				InstancePtr->Config.MaxLaneCount);

		/* Wait for us */
		XDp_WaitUs(InstancePtr->DpPtr, 1000);

		/* Wait for us */
		XDp_WaitUs(InstancePtr->DpPtr, 1000);

		/* Initialize default training pattern callbacks in DP RX
		 * Subsystem
		 */
		XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_TP1,
				StubTp1Callback, (void *)InstancePtr);
		XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_TP2,
				StubTp2Callback, (void *)InstancePtr);
		XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_TP3,
				StubTp2Callback, (void *)InstancePtr);
		XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_UNPLUG,
				StubUnplugCallback, (void *)InstancePtr);
		if ((InstancePtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4)
		   || (InstancePtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_2_1)) {
			XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_TP4,
					StubTp2Callback, (void *)InstancePtr);
			XDp_RxSetCallback(InstancePtr->DpPtr, XDP_RX_HANDLER_ACCESS_LANE_SET,
					StubAccessLaneSetCallback, (void *)InstancePtr);
		}

		/* Initialize configurable parameters */
		InstancePtr->UsrOpt.Bpc = InstancePtr->Config.MaxBpc;
		InstancePtr->UsrOpt.LaneCount =
					InstancePtr->Config.MaxLaneCount;
		InstancePtr->UsrOpt.LinkRate =
					InstancePtr->DpPtr->Config.MaxLinkRate;
		InstancePtr->UsrOpt.MstSupport =
				InstancePtr->Config.MstSupport;

		/* Populate the RX core's ports with default values */
		DpRxSs_PopulateDpRxPorts(InstancePtr);
	}

#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
			(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
	/* Check for Timer Counter availability */
	XTmrCtr_Config *ConfigPtr;
	if (InstancePtr->TmrCtrPtr != NULL) {
		xdbg_printf(XDBG_DEBUG_GENERAL,"SS INFO: Initializing Timer "
				"Counter IP \n\r");
#ifndef SDT
		ConfigPtr = XTmrCtr_LookupConfig(
				InstancePtr->Config.TmrCtrSubCore.
				TmrCtrConfig.DeviceId);
#else
		ConfigPtr = XTmrCtr_LookupConfig(
				InstancePtr->Config.TmrCtrSubCore.
				TmrCtrConfig.BaseAddress);
#endif
		if (!ConfigPtr) {
			return XST_DEVICE_NOT_FOUND;
		}

		/* Calculate absolute base address of Timer Counter sub-core */
		XTmrCtr_CfgInitialize(InstancePtr->TmrCtrPtr, ConfigPtr,
				ConfigPtr->BaseAddress +
				InstancePtr->Config.BaseAddress);
		Status = XTmrCtr_InitHw(InstancePtr->TmrCtrPtr);
		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_GENERAL,"SS ERR:: Timer "
					"Counter initialization failed\n\r");
			return XST_FAILURE;
		}

		/* Configure the callback */
		XTmrCtr_SetHandler(InstancePtr->TmrCtrPtr,
				(XTmrCtr_Handler)DpRxSs_TimeOutCallback,
				InstancePtr);

		/* Enable the specified options for Timer Counter zero */
		XTmrCtr_SetOptions(InstancePtr->TmrCtrPtr, 0,
				XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION);

		/* Set the reset value to Timer Counter zero */
		XTmrCtr_SetResetValue(InstancePtr->TmrCtrPtr, 0,
				XDPRXSS_TMRCTR_RST_VAL);
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
		if (InstancePtr->Hdcp1xPtr != NULL) {
			/* Initialize the HDCP timer functions */
			XHdcp1x_SetTimerStart(InstancePtr->Hdcp1xPtr,
					&DpRxSs_HdcpStartTimer);
			XHdcp1x_SetTimerStop(InstancePtr->Hdcp1xPtr,
					&DpRxSs_HdcpStopTimer);
			XHdcp1x_SetTimerDelay(InstancePtr->Hdcp1xPtr,
					&DpRxSs_HdcpBusyDelay);
		}
#endif
	}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	/* Check for HDCP availability */
	if ((InstancePtr->Hdcp1xPtr != NULL) &&
					(InstancePtr->Config.HdcpEnable)) {
		xdbg_printf(XDBG_DEBUG_GENERAL,"SS INFO: Initializing HDCP IP "
				"\n\r");
#ifndef SDT
		Hdcp1xConfig = XHdcp1x_LookupConfig(
				InstancePtr->Config.Hdcp1xSubCore.Hdcp1xConfig.DeviceId);
#else
		Hdcp1xConfig = XHdcp1x_LookupConfig(
				InstancePtr->Config.Hdcp1xSubCore.Hdcp1xConfig.BaseAddress);
#endif
		if (!Hdcp1xConfig) {
			return XST_DEVICE_NOT_FOUND;
		}

		/* Calculate absolute base address of HDCP sub-core */
		Hdcp1xConfig->BaseAddress += InstancePtr->Config.BaseAddress;
		/* HDCP1x config initialize */
		Status = XHdcp1x_CfgInitialize(InstancePtr->Hdcp1xPtr,
				Hdcp1xConfig, (void *)InstancePtr->DpPtr,
				Hdcp1xConfig->BaseAddress);
		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_GENERAL,"SS ERR:: HDCP "
				"initialization failed\n\r");
			return XST_FAILURE;
		}

		/* Set key selection value for RX */
		XHdcp1x_SetKeySelect(InstancePtr->Hdcp1xPtr, 0x1);

		XHdcp1x_LateInit(InstancePtr->Hdcp1xPtr);
	}
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/* HDCP 2.2 */
	if (InstancePtr->Hdcp22Ptr && InstancePtr->Config.Hdcp22Enable)
	{
		xdbg_printf(XDBG_DEBUG_GENERAL,
				"SS INFO: Initializing HDCP22 IP\n\r");
		if (XDpRxSs_SubcoreInitHdcp22((void *)InstancePtr) !=
				XST_SUCCESS)
		{
			xdbg_printf(XDBG_DEBUG_GENERAL,
				"DPRXSS ERR:: Initializing HDCP22 IP failed"
				"\n\r");
			return(XST_FAILURE);
		}

		XHdcp22Rx_Dp_SetHdcp22OverProtocol(InstancePtr->Hdcp22Ptr,
				XHDCP22_RX_DP);
	}
#endif

#if ((XPAR_XHDCP_NUM_INSTANCES > 0) || (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0))
	/* Default value */
	InstancePtr->HdcpIsReady = FALSE;
#endif

#if ((XPAR_DPRXSS_0_HDCP_ENABLE > 0) && (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0))
	/*
	 * Set default HDCP protocol.
	 * Setting HDCP1x as default if both HDCP1x
	 * and HDCP22 are capable & enabled
	 */

	/*
	 * HDCP is ready when only the HDCP 1.4 core is
	 * instantiated and the keys are loaded
	 */
	if (InstancePtr->Hdcp1xPtr) {
		InstancePtr->HdcpIsReady = TRUE;

		/* Set default HDCP content protection scheme */
		if (XDpRxSs_HdcpSetProtocol(InstancePtr, XDPRXSS_HDCP_14)) {
			xdbg_printf(XDBG_DEBUG_GENERAL,
					"DPRXSS ERR:: setting HDCP14 as default"
					"protocol\n\r");
			return XST_FAILURE;
		}
	}
#endif
#if (XPAR_DPRXSS_0_HDCP_ENABLE > 0)
	/*
	 * HDCP1X.
	 * HDCP is ready when only the HDCP 1.4 core is
	 * instantiated and the key is loaded
	 */
	if (InstancePtr->Hdcp1xPtr) {
		InstancePtr->HdcpIsReady = TRUE;

		/* Set default HDCP content protection scheme */
		XDpRxSs_HdcpSetProtocol(InstancePtr, XDPRXSS_HDCP_14);
	}
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/*HDCP22*/
	if (InstancePtr->Hdcp22Ptr &&
			InstancePtr->Hdcp22Lc128Ptr &&
			InstancePtr->Hdcp22PrivateKeyPtr) {
		InstancePtr->HdcpIsReady = TRUE;

		/* Set default HDCP content protection scheme */
		XDpRxSs_HdcpSetProtocol(InstancePtr, XDPRXSS_HDCP_22);
	}
#endif

	/* Set the flag to indicate the subsystem is ready */
	InstancePtr->IsReady = (u32)(XIL_COMPONENT_IS_READY);

	XDpRxSs_SetCallBack(InstancePtr, XDPRXSS_DRV_HANDLER_DP_NO_VID_EVENT,
				XDpRxSs_DrvNoVideoHandler, InstancePtr);
	XDpRxSs_SetCallBack(InstancePtr, XDPRXSS_DRV_HANDLER_DP_VID_EVENT,
				XDpRxSs_DrvVideoHandler, InstancePtr);
	XDpRxSs_SetCallBack(InstancePtr, XDPRXSS_DRV_HANDLER_DP_PWR_CHG_EVENT,
				XDpRxSs_DrvPowerChangeHandler, InstancePtr);
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function resets the DisplayPort Receiver Subsystem including all
* sub-cores.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		IIC needs to be reinitialized after reset.
*
******************************************************************************/
void XDpRxSs_Reset(XDpRxSs *InstancePtr)
{
	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	/* Reset HDCP interface */
	if ((InstancePtr->Hdcp1xPtr) && (InstancePtr->Config.HdcpEnable)) {
		XHdcp1x_Reset(InstancePtr->Hdcp1xPtr);
	}
#endif
#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
	(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
	/* Reset Timer Counter zero */
	if (InstancePtr->TmrCtrPtr) {
		XTmrCtr_Reset(InstancePtr->TmrCtrPtr, 0);
	}
#endif
	/* Reset the video and AUX logic from DP RX */
	XDpRxSs_WriteReg(InstancePtr->DpPtr->Config.BaseAddr,
			XDP_RX_SOFT_RESET, XDP_RX_SOFT_RESET_VIDEO_MASK |
				XDP_RX_SOFT_RESET_AUX_MASK);
	/* Reset the IIC core */
#ifdef XPAR_XIIC_NUM_INSTANCES
	if (InstancePtr->Config.IncludeAxiIic && InstancePtr->IicPtr) {
		XIic_Reset(InstancePtr->IicPtr);
	}
	else
#endif
	{
#ifdef XPAR_XIICPS_NUM_INSTANCES
		if (InstancePtr->IicPsPtr) {
			/* Reset the IIC core */
			XIicPs_Reset(InstancePtr->IicPsPtr);
		}
#endif
	}
}

/*****************************************************************************/
/**
*
* This function starts the DisplayPort Receiver Subsystem including all
* sub-cores.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if DP RX Subsystem and its included sub-cores
*		configured successfully.
*		- XST_FAILURE, otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_Start(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((InstancePtr->UsrOpt.MstSupport == 0) ||
				(InstancePtr->UsrOpt.MstSupport == 1));

	/* Reinitialize DP */
	Status = XDp_Initialize(InstancePtr->DpPtr);
	if (Status != (XST_SUCCESS)) {
		xdbg_printf((XDBG_DEBUG_GENERAL),"SS ERR::DP RX "
			"start failed!\n\r");
		Status = XST_FAILURE;
	}

	return Status;
}

/*****************************************************************************/
/**
*
* This function sets the data rate to be used by the DisplayPort RX Subsystem
* core.
*
* @param	InstancePtr is a pointer to the XDpRxSs instance.
* @param	LinkRate is the rate at which link needs to be driven.
*		- XDPRXSS_LINK_BW_SET_162GBPS = 0x06(for a 1.62 Gbps data rate)
*		- XDPRXSS_LINK_BW_SET_270GBPS = 0x0A(for a 2.70 Gbps data rate)
*		- XDPRXSS_LINK_BW_SET_540GBPS = 0x14(for a 5.40 Gbps data rate)
*
* @return
*		- XST_SUCCESS if setting the new lane rate was successful.
*		- XST_FAILURE otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_SetLinkRate(XDpRxSs *InstancePtr, u8 LinkRate)
{
	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	if (InstancePtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4) {
		Xil_AssertNonvoid((LinkRate == (XDPRXSS_LINK_BW_SET_162GBPS)) ||
				(LinkRate == (XDPRXSS_LINK_BW_SET_270GBPS)) ||
				(LinkRate == (XDPRXSS_LINK_BW_SET_540GBPS)) ||
				(LinkRate == (XDPRXSS_LINK_BW_SET_810GBPS)));

		/* Check for maximum supported link rate */
		if (LinkRate > InstancePtr->DpPtr->Config.MaxLinkRate) {
			xdbg_printf((XDBG_DEBUG_GENERAL),"SS info: This link rate is "
			"not supported by Source/Sink.\n\rMax Supported link "
			"rate is 0x%x.\n\rSetting maximum supported link "
			"rate.\n\r", InstancePtr->DpPtr->Config.MaxLinkRate);
			LinkRate = InstancePtr->DpPtr->Config.MaxLinkRate;
		}
	}

	/* Set link rate */
	XDp_RxSetLinkRate(InstancePtr->DpPtr, LinkRate);
	InstancePtr->UsrOpt.LinkRate = LinkRate;

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function sets the number of lanes to be used by DisplayPort RX Subsystem
* core.
*
* @param	InstancePtr is a pointer to the XDpRxSs instance.
* @param	LaneCount is the number of lanes to be used.
*		- 1 = XDPRXSS_LANE_COUNT_SET_1
*		- 2 = XDPRXSS_LANE_COUNT_SET_2
*		- 4 = XDPRXSS_LANE_COUNT_SET_4
* @return
*		- XST_SUCCESS if setting the new lane count was successful.
*		- XST_FAILURE otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_SetLaneCount(XDpRxSs *InstancePtr, u8 LaneCount)
{
	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((LaneCount == (XDPRXSS_LANE_COUNT_SET_1)) ||
			(LaneCount == (XDPRXSS_LANE_COUNT_SET_2)) ||
			(LaneCount == (XDPRXSS_LANE_COUNT_SET_4)));

	/* Check for maximum supported lane count */
	if (LaneCount > InstancePtr->DpPtr->Config.MaxLaneCount) {
		xdbg_printf((XDBG_DEBUG_GENERAL),"SS info: This lane count is "
			"not supported by Source/Sink.\n\rMax Supported lane "
			"count is 0x%x.\n\rSetting maximum supported lane "
			"count.\n\r", InstancePtr->DpPtr->Config.MaxLaneCount);
		LaneCount = InstancePtr->DpPtr->Config.MaxLaneCount;
	}

	/* Set lane count */
	XDp_RxSetLaneCount(InstancePtr->DpPtr, LaneCount);
	InstancePtr->UsrOpt.LaneCount = LaneCount;

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function allows the user to select number of ports to be exposed when
* replying to a LINK_ADDRESS sideband message and hides rest of the ports.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	Port specifies the number of ports to be exposed within the
*		range 1 to 4.
*
* @return
*		- XST_SUCCESS, if ports exposed successfully.
*		- XST_FAILURE, if exposing ports which are already exposed or
*		ports are exceeding total number of stream supported by the
*		system.
*
* @note		Number of ports are equal to number of streams.
*
******************************************************************************/
u32 XDpRxSs_ExposePort(XDpRxSs *InstancePtr, u8 Port)
{
	u32 Status;
	u8 PortIndex;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Port > 0x0);

	if (Port == InstancePtr->UsrOpt.NumOfStreams) {
		xdbg_printf(XDBG_DEBUG_GENERAL,"SS INFO:Subsystem is "
			"already in %s mode with port[s] %d.\n\r",
				InstancePtr->UsrOpt.MstSupport? "MST": "SST",
					Port);
		Status = XST_FAILURE;
	}
	/* Check for stream[s] less than supported stream[s] */
	else if (Port < InstancePtr->Config.NumMstStreams) {
		/* Expose each port */
		for (PortIndex = 0; PortIndex < Port; PortIndex++) {
			/* Expose the ports configured above. Used when
			 * replying to a LINK_ADDRESS request. Make sure that
			 * the number of downstream ports matches the number
			 * exposed, otherwise the LINK_ADDRESS reply will be
			 * incorrect
			 */
			XDp_RxMstExposePort(InstancePtr->DpPtr,
				PortIndex + 1, 1);
		}

		/* Set number of ports/stream[s] are exposed */
		InstancePtr->UsrOpt.NumOfStreams = Port;

		/* Hide remaining ports */
		for (PortIndex = Port;
			PortIndex <= InstancePtr->Config.NumMstStreams;
								PortIndex++) {
			XDp_RxMstExposePort(InstancePtr->DpPtr,
				PortIndex + 1, 0);
		}

		Status = XST_SUCCESS;
	}
	/* Everything else */
	else {
		xdbg_printf((XDBG_DEBUG_GENERAL),"SS ERR::Subsystem does not "
			"support %s mode with stream[s] %d\n\r",
				InstancePtr->UsrOpt.MstSupport? "MST": "SST",
					Port);
		Status = XST_FAILURE;
	}

	return Status;
}

/*****************************************************************************/
/**
*
* This function checks if the receiver's DisplayPort Configuration Data (DPCD)
* indicates that the receiver has achieved clock recovery, channel
* equalization, symbol lock, and interlane alignment for all lanes currently
* in use.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS if the RX device has achieved clock recovery,
*		  channel equalization, symbol lock, and interlane alignment.
*		- XST_DEVICE_NOT_FOUND if no RX device is connected.
*		- XST_FAILURE otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_CheckLinkStatus(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify argument.*/
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Check link status */
	Status = XDp_RxCheckLinkStatus(InstancePtr->DpPtr);

	return Status;
}

/*****************************************************************************/
/**
*
* This function configures the number of pixels output through the user data
* interface.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	UserPixelWidth is the user pixel width to be configured.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_SetUserPixelWidth(XDpRxSs *InstancePtr, u8 UserPixelWidth)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid((UserPixelWidth == 1) || (UserPixelWidth == 2) ||
			(UserPixelWidth == 4));

	/* Set user pixel width */
	XDp_RxSetUserPixelWidth(InstancePtr->DpPtr, UserPixelWidth);
}

/******************************************************************************/
/**
 * This function extracts the bits per color from MISC0 or VSC SDP packet based
 * on whether reception of colorimetry information through VSC SDP packets or
 * through MISC registers of the stream.
 *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @param	Stream is the stream number to make the calculations for.
 *
 * @return	The bits per color for the stream.
 *
 * @note	RX clock must be stable.
 *
*******************************************************************************/
u8 XDpRxss_GetBpc(XDpRxSs *InstancePtr, u8 Stream)
{
	u8 Bpc;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
						(Stream == XDP_TX_STREAM_ID2) ||
						(Stream == XDP_TX_STREAM_ID3) ||
						(Stream == XDP_TX_STREAM_ID4));

	Bpc = XDp_RxGetBpc(InstancePtr->DpPtr, Stream);

	return Bpc;
}

/******************************************************************************/
/**
 * This function extracts the incoming video stream is Progressive or Interlace mode. *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @param	Stream is the stream number to make the calculations for.
 *
 * @return	Video timing is Progressive or Interlace.
 *
 * @note	RX clock must be stable.
 *
 *******************************************************************************/
u8 XDpRxss_GetInterlace(XDpRxSs *InstancePtr, u8 Stream)
{
	u8 interlace;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
			  (Stream == XDP_TX_STREAM_ID2) ||
			  (Stream == XDP_TX_STREAM_ID3) ||
			  (Stream == XDP_TX_STREAM_ID4));

	interlace =
		XDp_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_VBID);

	interlace = interlace >> 1;

	return interlace;
}

/******************************************************************************/
/**
 * This function extracts the color component format from MISC0 or VSC SDP packet
 * based on whether reception of colorimetry information through VSC SDP packets or
 * through MISC registers of the stream.
 *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @param	Stream is the stream number to make the calculations for.
 *
 * @return	The color component for the stream.
 *
 * @note	RX clock must be stable.
 *
*******************************************************************************/
u8 XDpRxss_GetColorComponent(XDpRxSs *InstancePtr, u8 Stream)
{
	u8 ColorComponent;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
						(Stream == XDP_TX_STREAM_ID2) ||
						(Stream == XDP_TX_STREAM_ID3) ||
						(Stream == XDP_TX_STREAM_ID4));

	ColorComponent = XDp_RxGetColorComponent(InstancePtr->DpPtr, Stream);

	return ColorComponent;
}

/******************************************************************************/
/**
 * This function extracts the YCbCrColorimetry from MISC0 or VSC SDP packet
 * based on whether reception of colorimetry information through VSC SDP packets
 * or through MISC registers of the stream.
 *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @param	Stream is the stream number to make the calculations for.
 *
 * @return	The colorimetry format for the stream.
 *
 * @note	RX clock must be stable.
 *
*******************************************************************************/
u8 XDpRxss_GetColorimetry(XDpRxSs *InstancePtr, u8 Stream)
{
	u8 YCbCrColorimetry;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
						(Stream == XDP_TX_STREAM_ID2) ||
						(Stream == XDP_TX_STREAM_ID3) ||
						(Stream == XDP_TX_STREAM_ID4));

	YCbCrColorimetry = XDp_RxGetColorimetry(InstancePtr->DpPtr, Stream);

	return YCbCrColorimetry;
}

/******************************************************************************/
/**
 * This function extracts the dynamic range from MISC0 or VSC SDP packet
 * based on whether reception of colorimetry information through VSC SDP packets or
 * through MISC registers of the stream.
 *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @param	Stream is the stream number to make the calculations for.
 *
 * @return	The dynamic range value for the stream.
 *
 * @note	RX clock must be stable.
 *
*******************************************************************************/
u8 XDpRxss_GetDynamicRange(XDpRxSs *InstancePtr, u8 Stream)
{
	u8 DynamicRange;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
						(Stream == XDP_TX_STREAM_ID2) ||
						(Stream == XDP_TX_STREAM_ID3) ||
						(Stream == XDP_TX_STREAM_ID4));

	DynamicRange = XDp_RxGetDynamicRange(InstancePtr->DpPtr, Stream);

	return DynamicRange;
}

/*****************************************************************************/
/**	 This function gives the main stream attributes (MSA).
 *
 *	@param		InstancePtr is a pointer to the XDpRxSs instance.
 *
 *	@return		MSA Config structure
 *
 *****************************************************************************/
XDp_MainStreamAttributes *XDPRxss_GetMsa(XDpRxSs *DpRxSsInst)
{
	u8 Stream;
	u32 DpHres = 0, i = 0;
	u32 DpVres = 0;
	u32 DpHres_total = 0;
	u32 DpVres_total = 0;
	u32 rxMsamisc0 = 0;
	u32 rxMsamisc1 = 0;
	u32 rxMsaMVid = 0;
	u32 rxMsaNVid = 0;
	u8 bpc;
	char *color;
	u8 color_mode = 0;
	XDp_MainStreamAttributes *Msa_Config =
			(XDp_MainStreamAttributes *)&DpRxSsInst->DpPtr->RxInstance.MsaConfig;

	for (Stream = 1; Stream <= DpRxSsInst->Config.NumMstStreams; Stream++) {
		while ((DpHres == 0 || i < 300) && DpRxSsInst->link_up_trigger == 1) {
			DpHres = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
					     XDP_RX_MSA_HRES);
			i++;
		}
		while ((DpVres == 0 || i < 300) && DpRxSsInst->link_up_trigger == 1) {
			DpVres = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
					     XDP_RX_MSA_VHEIGHT);
			i++;
		}

		DpHres_total =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_HTOTAL);
		DpVres_total =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_VTOTAL);
		rxMsamisc0 =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_MISC0);
		rxMsamisc1 =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_MISC1);
		rxMsaMVid =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_MVID);
		rxMsaNVid =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_NVID);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Misc0 = rxMsamisc0;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Misc1 = rxMsamisc1;

		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.HActive = DpHres;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.VActive = DpVres;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.HTotal =
									DpHres_total;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.F0PVTotal =
									DpVres_total;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].MVid = rxMsaMVid;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].NVid = rxMsaNVid;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].HStart =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_HSTART);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].VStart =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_VSTART);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.HSyncWidth =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_HSWIDTH);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.F0PVSyncWidth =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_VSWIDTH);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.HSyncPolarity =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_HSPOL);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.Timing.VSyncPolarity =
		XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
			    XDP_RX_STREAM_MSA_OFFSET((Stream - 1)) + XDP_RX_INDIVIDUAL_MSA_VSPOL);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].SynchronousClockMode =
				rxMsamisc0 & 1;

		bpc = XDpRxss_GetBpc(DpRxSsInst, XDP_TX_STREAM_ID1);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].BitsPerColor = bpc;

		color_mode = XDpRxss_GetColorComponent(DpRxSsInst, Stream);

		/* Calculate the pixel clock frequency based on channel coding set(8/10b or 128/132b) */
		u32 Channel_Coding = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
									XDP_RX_DPCD_MAIN_LINK_CHANNEL_CODING_SET);
		u32 recv_clk_freq=0;
		float recv_frame_clk=0;

		if((Channel_Coding & 0x3) == 0x1){ //dp1.4 linkrates
			recv_clk_freq =
					(((int)DpRxSsInst->UsrOpt.LinkRate*27)*rxMsaMVid)/rxMsaNVid;

			recv_frame_clk =
				(int)( (recv_clk_freq*1000000.0)/(DpHres_total * DpVres_total) < 0.0 ?
						(recv_clk_freq*1000000.0)/(DpHres_total * DpVres_total) :
						(recv_clk_freq*1000000.0)/(DpHres_total * DpVres_total)+0.9
						);
			DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].IsRxDp21 = 0;
		}else{	//dp2.1 linkrates
			u32 VFreq_lower = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
										0x1608);
			u32 VFreq_higher = XDp_ReadReg(DpRxSsInst->DpPtr->Config.BaseAddr,
										0x160C);
			u64 VFreq = 0;
			VFreq = VFreq_higher;
			VFreq = (VFreq << 24) | VFreq_lower;
			DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].VFreq = VFreq;
		    DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].IsRxDp21 = 1;

			recv_frame_clk =
				(int)( (VFreq*1.0)/(DpHres_total * DpVres_total) < 0.0 ?
						(VFreq*1.0)/(DpHres_total * DpVres_total) :
						(VFreq*1.0)/(DpHres_total * DpVres_total)+0.9
						);
		}

		XVidC_FrameRate recv_frame_clk_int = recv_frame_clk;
		//Doing Approximation here
		if (recv_frame_clk_int == 49 || recv_frame_clk_int == 51) {
			recv_frame_clk_int = 50;
		} else if (recv_frame_clk_int == 59 || recv_frame_clk_int == 61) {
			recv_frame_clk_int = 60;
		} else if (recv_frame_clk_int == 29 || recv_frame_clk_int == 31) {
			recv_frame_clk_int = 30;
		} else if (recv_frame_clk_int == 76 || recv_frame_clk_int == 74) {
			recv_frame_clk_int = 75;
		} else if (recv_frame_clk_int == 121 || recv_frame_clk_int == 119) {
			recv_frame_clk_int = 120;
		}


		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].PixelClockHz = DpHres_total * DpVres_total * recv_frame_clk_int;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.FrameRate =recv_frame_clk_int;
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].DynamicRange = XDpRxss_GetDynamicRange(DpRxSsInst, Stream);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].YCbCrColorimetry = XDpRxss_GetColorimetry(DpRxSsInst, Stream);
		DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.VmId =
		XVidC_GetVideoModeId(DpHres, DpVres,
				     DpRxSsInst->DpPtr->RxInstance.MsaConfig[(Stream - 1)].Vtm.FrameRate,
				     XDpRxss_GetInterlace(DpRxSsInst, Stream));

		if (color_mode == 0) {
			DpRxSsInst->DpPtr->RxInstance.MsaConfig[Stream -1].ComponentFormat =
					XDP_MAIN_STREAMX_MISC0_COMPONENT_FORMAT_RGB;
			color = "RGB";
		} else if (color_mode == 1) {
			DpRxSsInst->DpPtr->RxInstance.MsaConfig[Stream - 1].ComponentFormat =
					XDP_MAIN_STREAMX_MISC0_COMPONENT_FORMAT_YCBCR444;
			color = "YCbCr444";
		} else if (color_mode == 2) {
			DpRxSsInst->DpPtr->RxInstance.MsaConfig[Stream - 1].ComponentFormat =
					XDP_MAIN_STREAMX_MISC0_COMPONENT_FORMAT_YCBCR422;
			color = "YCbCr422";
		} else {
			//RAW, 420, Y unsupported
			xil_printf("Unsupported Color Format\r\n");
		}
	}

	return Msa_Config;
}

/*****************************************************************************/
/**
 *
 * This function retrieves the VideoStream Parameters from
 * the incoming video stream.
 *
 * @param	InstancePtr is a pointer to the XDpRxSs core instance.
 * @Stream	Stream is the stream number to get the Video data.
 *
 * @return
 *		- SUCCESS when video data extraction is success
 *
 ******************************************************************************/
int XDpRxSs_GetVideoStream(XDpRxSs *InstancePtr, XVidC_VideoStream *VideoStream, u8 Stream)
{
	XDp_MainStreamAttributes *Msa_Config;
	u32 VTotal;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
			  (Stream == XDP_TX_STREAM_ID2) ||
			  (Stream == XDP_TX_STREAM_ID3) ||
			  (Stream == XDP_TX_STREAM_ID4));

	u32 StreamOffset[4] = {XDP_RX_STREAM1_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM2_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM3_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM4_ADAPTIVE_VBLANK_VTOTAL};

	Msa_Config = XDPRxss_GetMsa(InstancePtr);

	VideoStream->Timing =
		InstancePtr->DpPtr->RxInstance.MsaConfig[Stream - 1].Vtm.Timing;
	VideoStream->FrameRate =
		InstancePtr->DpPtr->RxInstance.MsaConfig[Stream - 1].Vtm.FrameRate;
	VideoStream->VmId =
		InstancePtr->DpPtr->RxInstance.MsaConfig[Stream - 1].Vtm.VmId;
	VideoStream->PixPerClk = InstancePtr->UsrOpt.LaneCount;
	VideoStream->ColorDepth =
		InstancePtr->DpPtr->RxInstance.MsaConfig[Stream - 1].BitsPerColor;
	VideoStream->ColorFormatId =
		InstancePtr->DpPtr->RxInstance.MsaConfig[Stream - 1].ComponentFormat;
	VideoStream->ColorStd = XDp_RxGetColorimetry(InstancePtr->DpPtr, Stream);
	VideoStream->DynamicRange = XDp_RxGetDynamicRange(InstancePtr->DpPtr, Stream);
	VideoStream->BaseFrameRate = VideoStream->FrameRate;
	VideoStream->BaseTiming = VideoStream->Timing;
	VideoStream->UncompressedTiming = VideoStream->Timing;
	VideoStream->IsInterlaced = XDpRxss_GetInterlace(InstancePtr, Stream);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function handles incoming sideband messages. It will
* 1) Read the contents of the down request registers,
* 2) Delegate control depending on the request type, and
* 3) Send a down reply.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS if the entire message was sent successfully.
*		- XST_DEVICE_NOT_FOUND if no device is connected.
*		- XST_ERROR_COUNT_MAX if sending one of the message fragments
*		timed out.
*		- XST_FAILURE otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_HandleDownReq(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify argument.*/
	Xil_AssertNonvoid(InstancePtr != NULL);

	Status = XDp_RxHandleDownReq(InstancePtr->DpPtr);

	return Status;
}

#if (XPAR_XHDCP_NUM_INSTANCES > 0) || (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
*
* This function enables the requested HDCP protocol. This function
* ensures that the HDCP protocols are mutually exclusive such that
* either HDCP 1.4 or HDCP 2.2 is enabled and active at any given time.
* When the protocol is set to None, both HDCP protocols are disabled.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if HDCP i/f enabled successfully.
*		- XST_FAILURE, otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_HdcpEnable(XDpRxSs *InstancePtr)
{
	u32 Status1 = XST_SUCCESS, Status2 = XST_SUCCESS;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr);
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr)
		Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable);
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp22Ptr)
		Xil_AssertNonvoid(InstancePtr->Config.Hdcp22Enable);
#endif

	switch (InstancePtr->HdcpProtocol) {
		/* Disable HDCP 1.4 and HDCP 2.2 */
		case XDPRXSS_HDCP_NONE :
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp1xPtr) {
				Status1 = XHdcp1x_Disable(
						InstancePtr->Hdcp1xPtr);
				/*  This is needed to ensure that the previous
				 *  command is executed */
				XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);
			}
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp22Ptr) {
				Status2 = XHdcp22Rx_Dp_Disable(
						InstancePtr->Hdcp22Ptr);
			}
#endif
			break;

			/* Enable HDCP 1.4 and disable HDCP 2.2 */
		case XDPRXSS_HDCP_14 :
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp1xPtr) {
				Status1 = XHdcp1x_Enable(
						InstancePtr->Hdcp1xPtr);
				/* This is needed to ensure that the previous
				 * command is executed */
				XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);
			}
			else {
				Status1 = XST_FAILURE;
			}
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp22Ptr) {
				Status2 = XHdcp22Rx_Dp_Disable(
						InstancePtr->Hdcp22Ptr);
			}
#endif
			break;

			/* Enable HDCP 2.2 and disable HDCP 1.4 */
		case XDPRXSS_HDCP_22 :
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp1xPtr) {
				Status1 = XHdcp1x_Disable(
						InstancePtr->Hdcp1xPtr);
				/* This is needed to ensure that the previous
				 * command is executed. */
				XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);
			}
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
			if (InstancePtr->Hdcp22Ptr) {
				Status2 = XHdcp22Rx_Dp_Enable(
						InstancePtr->Hdcp22Ptr);
			}
			else {
				Status2 = XST_FAILURE;
			}
#endif
			break;

		default :
			return XST_FAILURE;
	}

	return (Status1 == XST_SUCCESS && Status2 == XST_SUCCESS) ?
		XST_SUCCESS : XST_FAILURE;
}

/*****************************************************************************/
/**
*
* This function disables High-Bandwidth Content Protection (HDCP) interface.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if HDCP i/f disabled successfully.
*		- XST_FAILURE, otherwise.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_HdcpDisable(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr);
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr)
		Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable);
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp22Ptr)
		Xil_AssertNonvoid(InstancePtr->Config.Hdcp22Enable);
#endif
	/* Set protocol to NONE then reset/disable HDCP 1X and 2.2 */
	InstancePtr->HdcpProtocol = XDPRXSS_HDCP_NONE;
	Status = XDpRxSs_HdcpReset(InstancePtr);

	return Status;
}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
*
* This function polls the HDCP interface, process events and sets receive
* state machine accordingly.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if polling the HDCP interface was successful.
*		- XST_FAILURE, if polling the HDCP interface failed.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_Poll(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Poll the HDCP interface */
	Status = XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);

	return Status;
}

/*****************************************************************************/
/**
*
* This function enables/disables the underlying physical interface.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	PhyState indicates TRUE/FALSE value to enable/disable the
*		underlying physical interface.
*
* @return
*		- XST_SUCCESS, if the underlying physical interface enabled
*		successfully.
*		- XST_FAILURE, if the underlying physical interface failed to
*		enable.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_SetPhysicalState(XDpRxSs *InstancePtr, u32 PhyState)
{
	u32 Status;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((PhyState == TRUE) || (PhyState == FALSE));
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Enable underlying physical interface */
	Status = XHdcp1x_SetPhysicalState(InstancePtr->Hdcp1xPtr, PhyState);

	return Status;
}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0) || (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
*
* This function sets lane(s) of the HDCP interface.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	Lane is the number of lanes to be used.
*		- 1 = XDPRXSS_LANE_COUNT_SET_1
*		- 2 = XDPRXSS_LANE_COUNT_SET_2
*		- 4 = XDPRXSS_LANE_COUNT_SET_4
*
* @return
*		- XST_SUCCESS, if lane(s) into the HDCP i/f set successfully.
*		- XST_FAILURE, if failed to set lane(s) into the HDCP i/f.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_SetLane(XDpRxSs *InstancePtr, u32 Lane)
{
	u32 Status = XST_SUCCESS;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr);
#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr)
		Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable);
#endif
#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp22Ptr)
		Xil_AssertNonvoid(InstancePtr->Config.Hdcp22Enable);
#endif
	Xil_AssertNonvoid((Lane == XDPRXSS_LANE_COUNT_SET_1) ||
			(Lane == XDPRXSS_LANE_COUNT_SET_2) ||
			(Lane == XDPRXSS_LANE_COUNT_SET_4));

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr) {
		/* Set lanes into the HDCP interface */
		Status = XHdcp1x_SetLaneCount(InstancePtr->Hdcp1xPtr, Lane);

		if (Status != XST_SUCCESS) {
			xdbg_printf(XDBG_DEBUG_GENERAL,
					"Set lane count is failed\n\r");
			return XST_FAILURE;
		}
	}
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp22Ptr)
		XHdcp22_Dp_RxSetLaneCount(InstancePtr->Hdcp22Ptr, Lane);
#endif

	return Status;
}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
*
* This function initiates authentication process.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if authentication initiated successfully.
*		- XST_FAILURE, if authentication initiated failed.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_Authenticate(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Initiate authentication process */
	Status = XHdcp1x_Authenticate(InstancePtr->Hdcp1xPtr);

	return Status;
}

/*****************************************************************************/
/**
*
* This function checks whether the HDCP Receiver authenticated or not.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- TRUE, if HDCP Receiver is authenticated.
*		- FALSE, if the HDCP Receiver not authenticated.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_IsAuthenticated(XDpRxSs *InstancePtr)
{
	u32 Authenticate;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Check authentication has completed successfully */
	Authenticate = XHdcp1x_IsAuthenticated(InstancePtr->Hdcp1xPtr);

	return Authenticate;
}

/*****************************************************************************/
/**
*
* This function retrieves the current encryption map.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- The current encryption map.
*		- Otherwise zero.
*
* @note		None.
*
******************************************************************************/
u64 XDpRxSs_GetEncryption(XDpRxSs *InstancePtr)
{
	u64 StreamMap;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Get stream map of the stream(s) */
	StreamMap = XHdcp1x_GetEncryption(InstancePtr->Hdcp1xPtr);

	return StreamMap;
}

/*****************************************************************************/
/**
*
* This function sets the debug printf function.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	PrintfFunc is the printf function.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_SetDebugPrintf(XDpRxSs *InstancePtr, XDpRxSs_Printf PrintfFunc)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr->Config.HdcpEnable == 0x1);
	Xil_AssertVoid(PrintfFunc != NULL);

	/* Set debug printf function */
	XHdcp1x_SetDebugPrintf(PrintfFunc);
}

/*****************************************************************************/
/**
*
* This function sets the debug log message function.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	LogFunc is the debug logging function.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_SetDebugLogMsg(XDpRxSs *InstancePtr, XDpRxSs_LogMsg LogFunc)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr->Config.HdcpEnable == 0x1);
	Xil_AssertVoid(LogFunc != NULL);

	/* Set debug log message function */
	XHdcp1x_SetDebugLogMsg(LogFunc);
}

/*****************************************************************************/
/**
*
* This function informs the HDCP state machine that the downstream interfaces
* of the Repeater are ready (enabled or authenticated).
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return
*		- XST_SUCCESS, if downstream is ready.
*		- XST_FAILURE, if downstream is not ready.
*
* @note		None.
*
******************************************************************************/
u32 XDpRxSs_DownstreamReady(XDpRxSs *InstancePtr)
{
	u32 Status;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Poll the HDCP interface */
	Status = XHdcp1x_DownstreamReady(InstancePtr->Hdcp1xPtr);

	return Status;
}
#endif

#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
	(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
/*****************************************************************************/
/**
*
* This function starts the Timer Counter in count down, interrupt mode.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_StartTimer(XDpRxSs *InstancePtr)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr);

	/* Enable the specified options for Timer Counter zero */
	XTmrCtr_SetOptions(InstancePtr->TmrCtrPtr, 0,
		XTC_DOWN_COUNT_OPTION | XTC_INT_MODE_OPTION);

	/* Set the reset value to Timer Counter zero */
	XTmrCtr_SetResetValue(InstancePtr->TmrCtrPtr, 0,
				XDPRXSS_TMRCTR_RST_VAL);

	/* Start Timer Counter 0 in count down mode */
	XTmrCtr_Start(InstancePtr->TmrCtrPtr, 0);
}

/*****************************************************************************/
/**
*
* This function stops the Timer Counter.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_StopTimer(XDpRxSs *InstancePtr)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr);

	/* Stop Timer Counter 0 in count down mode */
	XTmrCtr_Stop(InstancePtr->TmrCtrPtr, 0);

	/* Reset Timer Counter reset done */
	InstancePtr->TmrCtrResetDone = 0;
}

/*****************************************************************************/
/**
*
* This function is the callback called when the Timer Counter reset done with
* specified reset value, assigned during initialization.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param	TmrCtrNumber is the number of the timer/counter within the
*			Timer Counter core.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void DpRxSs_TimeOutCallback(void *InstancePtr, u8 TmrCtrNumber)
{
	XDpRxSs *XDpRxSsPtr = (XDpRxSs *)InstancePtr;

	/* Verify arguments.*/
	Xil_AssertVoid(XDpRxSsPtr != NULL);
	Xil_AssertVoid(TmrCtrNumber < XTC_DEVICE_TIMER_COUNT);

	/* Set Timer Counter reset done */
	XDpRxSsPtr->TmrCtrResetDone = 1;

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/* Call HDCP22 Timer handler */
	if (XDpRxSsPtr->Hdcp22Ptr)
		XHdcp22Rx_Dp_TimerHandler((void *)XDpRxSsPtr->Hdcp22Ptr,
					  TmrCtrNumber);
#endif
}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
*
* This function handles a timeout for HDCP.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_HandleTimeout(XDpRxSs *InstancePtr)
{
	/* Verify arguments.*/
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->Config.HdcpEnable == 0x1);

	/* Handle timeout */
	XHdcp1x_HandleTimeout(InstancePtr->Hdcp1xPtr);
}

/*****************************************************************************/
/**
*
* This function starts a timer on behalf of an HDCP interface.
*
* @param	InstancePtr is a pointer to the XHdcp1x core instance.
* @param	TimeoutInMs the timer duration in milliseconds.
*
* @return
*		- XST_SUCCESS if Timer Counter started successfully.
*
* @note		None.
*
******************************************************************************/
static int DpRxSs_HdcpStartTimer(void *InstancePtr, u16 TimeoutInMs)
{
	XHdcp1x *HdcpPtr = (XHdcp1x *)InstancePtr;
	XTmrCtr *TmrCtrPtr = (XTmrCtr *)HdcpPtr->Hdcp1xRef;
	u8 TimerChannel;
	u32 TimerOptions;
	u32 NumTicks;

	/* Verify argument. */
	Xil_AssertNonvoid(HdcpPtr != NULL);
	Xil_AssertNonvoid(TmrCtrPtr != NULL);

	/* Determine NumTicks */
	NumTicks = DpRxSs_ConvertUsToTicks((TimeoutInMs * 1000),
				TmrCtrPtr->Config.SysClockFreqHz);

	/* Stop Timer Counter */
	TimerChannel = 0;
	XTmrCtr_Stop(TmrCtrPtr, TimerChannel);

	/* Configure the callback */
	XTmrCtr_SetHandler(TmrCtrPtr, &DpRxSs_TimerCallback,
				(void *)HdcpPtr);

	/* Configure the timer options */
	TimerOptions = XTmrCtr_GetOptions(TmrCtrPtr, TimerChannel);
	TimerOptions |= XTC_DOWN_COUNT_OPTION;
	TimerOptions |= XTC_INT_MODE_OPTION;
	TimerOptions &= ~XTC_AUTO_RELOAD_OPTION;
	XTmrCtr_SetOptions(TmrCtrPtr, TimerChannel, TimerOptions);

	/* Set the timeout and start */
	XTmrCtr_SetResetValue(TmrCtrPtr, TimerChannel, NumTicks);
	XTmrCtr_Start(TmrCtrPtr, TimerChannel);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function stops a timer on behalf of an HDCP interface
*
* @param	InstancePtr is a pointer to the XHdcp1x core instance.
*
* @return
*		- XST_SUCCESS if Timer Counter stopped successfully.
*
* @note		None.
*
******************************************************************************/
static int DpRxSs_HdcpStopTimer(void *InstancePtr)
{
	XHdcp1x *HdcpPtr = (XHdcp1x *)InstancePtr;
	XTmrCtr *TmrCtrPtr = (XTmrCtr *)HdcpPtr->Hdcp1xRef;
	u8 TimerChannel;

	/* Verify argument. */
	Xil_AssertNonvoid(TmrCtrPtr != NULL);

	/* Stop Timer Counter */
	TimerChannel = 0;
	XTmrCtr_Stop(TmrCtrPtr, TimerChannel);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function busy waits for an interval on behalf of an HDCP interface.
*
* @param	InstancePtr is a pointer to the XHdcp1x core instance.
* @param	DelayInMs the delay duration in milliseconds.
*
* @return
*		- XST_SUCCESS if Timer Counter busy wait successfully.
*
* @note		None.
*
******************************************************************************/
static int DpRxSs_HdcpBusyDelay(void *InstancePtr, u16 DelayInMs)
{
	XHdcp1x *HdcpPtr = (XHdcp1x *)InstancePtr;
	XTmrCtr *TmrCtrPtr = (XTmrCtr *)HdcpPtr->Hdcp1xRef;
	u8 TimerChannel;
	u32 TimerOptions;
	u32 NumTicks;

	/* Verify argument. */
	Xil_AssertNonvoid(TmrCtrPtr != NULL);

	/* Determine number of timer ticks */
	NumTicks = DpRxSs_ConvertUsToTicks((DelayInMs * 1000),
				TmrCtrPtr->Config.SysClockFreqHz);

	/* Stop it */
	TimerChannel = 0;
	XTmrCtr_Stop(TmrCtrPtr, TimerChannel);

	/* Configure the timer options */
	TimerOptions = XTmrCtr_GetOptions(TmrCtrPtr, TimerChannel);
	TimerOptions |= XTC_DOWN_COUNT_OPTION;
	TimerOptions &= ~XTC_INT_MODE_OPTION;
	TimerOptions &= ~XTC_AUTO_RELOAD_OPTION;
	XTmrCtr_SetOptions(TmrCtrPtr, TimerChannel, TimerOptions);

	/* Set the timeout and start */
	XTmrCtr_SetResetValue(TmrCtrPtr, TimerChannel, NumTicks);
	XTmrCtr_Start(TmrCtrPtr, TimerChannel);

	/* Wait until done */
	while (!XTmrCtr_IsExpired(TmrCtrPtr, TimerChannel));

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function serves as the timer callback.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @param 	TmrCtrNumber is the number of the timer/counter within the
*		device. The device typically contains at least two
*		timer/counters.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void DpRxSs_TimerCallback(void *InstancePtr, u8 TmrCtrNumber)
{
	XHdcp1x *Hdcp1xPtr = (XHdcp1x *)InstancePtr;

	/* Verify arguments. */
	Xil_AssertVoid(Hdcp1xPtr != NULL);
	Xil_AssertVoid(TmrCtrNumber < XTC_DEVICE_TIMER_COUNT);

	/* Handle timeout */
	XHdcp1x_HandleTimeout(Hdcp1xPtr);
}

/*****************************************************************************/
/**
*
* This function converts from microseconds to timer ticks.
*
* @param	TimeoutInUs is the timeout value to convert into timer ticks.
* @param 	ClkFreq the clock frequency to use in the conversion.
*
* @return	The number of timer ticks.
*
* @note		None.
*
******************************************************************************/
static u32 DpRxSs_ConvertUsToTicks(u32 TimeoutInUs, u32 ClkFreq)
{
	u32 TimeoutFreq;
	u32 NumSeconds;
	u32 NumTicks = 0;

	/* Check for greater than one second */
	if (TimeoutInUs > 1000000) {
		/* Determine the number of seconds */
		NumSeconds = (TimeoutInUs / 1000000);

		/* Update theNumTicks */
		NumTicks = (NumSeconds * ClkFreq);

		/* Adjust the TimeoutInUs */
		TimeoutInUs -= (NumSeconds * 1000000);
	}

	/* Convert TimeoutFreq to a frequency */
	TimeoutFreq = 1000;
	TimeoutFreq *= 1000;
	TimeoutFreq /= TimeoutInUs;

	/* Update NumTicks */
	NumTicks += ((ClkFreq / TimeoutFreq) + 1);

	return NumTicks;
}
#endif

/*****************************************************************************/
/**
*
* This function reports list of cores included in DisplayPort RX Subsystem.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
#ifndef SDT
static void DpRxSs_GetIncludedSubCores(XDpRxSs *InstancePtr)
{
	/* Assign instance of DisplayPort core */
	InstancePtr->DpPtr = ((InstancePtr->Config.DpSubCore.IsPresent) ?
		(&DpRxSsSubCores[InstancePtr->Config.DeviceId].DpInst) : NULL);
#ifdef XPAR_XIIC_NUM_INSTANCES
	if (InstancePtr->Config.IncludeAxiIic) {
	/* Assign instance of IIC core */
		InstancePtr->IicPtr =
			((InstancePtr->Config.DpSubCore.IsPresent) ?
			(&DpRxSsSubCores[InstancePtr->Config.DeviceId].IicInst)
			: NULL);
	}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	/* Assign instance of HDCP core */
	InstancePtr->Hdcp1xPtr =
		((InstancePtr->Config.Hdcp1xSubCore.IsPresent) ?
	(&DpRxSsSubCores[InstancePtr->Config.DeviceId].Hdcp1xInst) : NULL);
#endif

#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
			(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
	/* Assign instance of Timer Counter core */
	InstancePtr->TmrCtrPtr =
		((InstancePtr->Config.TmrCtrSubCore.IsPresent) ?
	(&DpRxSsSubCores[InstancePtr->Config.DeviceId].TmrCtrInst) : NULL);
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr != NULL)
		InstancePtr->Hdcp1xPtr->Hdcp1xRef =
			(void *)InstancePtr->TmrCtrPtr;
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/*Assign Instance of HDCP22 core*/
	InstancePtr->Hdcp22Ptr =
		((InstancePtr->Config.Hdcp22SubCore.IsPresent) ?
		 (&DpRxSsSubCores[InstancePtr->Config.DeviceId].Hdcp22Inst) :
		 NULL);
#endif
}
#else
static void DpRxSs_GetIncludedSubCores(XDpRxSs *InstancePtr, UINTPTR BaseAddress)
{
    u32 Index;

    Index = XDpRxSs_GetDrvIndex(BaseAddress);
	/* Assign instance of DisplayPort core */
	InstancePtr->DpPtr = ((InstancePtr->Config.DpSubCore.IsPresent) ?
		(&DpRxSsSubCores[Index].DpInst) : NULL);
#ifdef XPAR_XIIC_NUM_INSTANCES
	if (InstancePtr->Config.IncludeAxiIic) {
	/* Assign instance of IIC core */
		InstancePtr->IicPtr =
			((InstancePtr->Config.DpSubCore.IsPresent) ?
			(&DpRxSsSubCores[Index].IicInst)
			: NULL);
	}
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	/* Assign instance of HDCP core */
	InstancePtr->Hdcp1xPtr =
		((InstancePtr->Config.Hdcp1xSubCore.IsPresent) ?
	(&DpRxSsSubCores[Index].Hdcp1xInst) : NULL);
#endif

#if (((XPAR_XHDCP_NUM_INSTANCES > 0) || \
			(XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)) \
		&& (XPAR_XTMRCTR_NUM_INSTANCES > 0))
	/* Assign instance of Timer Counter core */
	InstancePtr->TmrCtrPtr =
		((InstancePtr->Config.TmrCtrSubCore.IsPresent) ?
	(&DpRxSsSubCores[Index].TmrCtrInst) : NULL);
#endif

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	if (InstancePtr->Hdcp1xPtr != NULL)
		InstancePtr->Hdcp1xPtr->Hdcp1xRef =
			(void *)InstancePtr->TmrCtrPtr;
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/*Assign Instance of HDCP22 core*/
	InstancePtr->Hdcp22Ptr =
		((InstancePtr->Config.Hdcp22SubCore.IsPresent) ?
		 (&DpRxSsSubCores[Index].Hdcp22Inst) :
		 NULL);
#endif
}
#endif
/*****************************************************************************/
/**
*
* This function sets up the downstream topology that will be used by the
* DisplayPort RX to respond to down requests coming from the TX.
* - Sinks equal to number of streams (each with its own global unique
*   identifier) are exposed to LINK_ADDRESS sideband messages.
* - The extended display identification data (EDID) will be set to a generic
*   EDID (GenEdid) for each of the sinks. REMOTE_I2C_READ sideband messages
*   on address 0x50 will be replied to with the contents set by
*   XDp_RxSetIicMapEntry for 0x50.
* - REMOTE_DPCD_READ sideband messages will be responded to with the contents
*   set for the sink using XDp_RxSetDpcdMap. All sinks are given the a generic
*   DPCD (GenDpcd).
* - ENUM_PATH_RESOURCES sideband messages will be responded to with the value
*   set by XDp_RxMstSetPbn (or 0 if the sink already has a stream allocated to
*   it).
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void DpRxSs_PopulateDpRxPorts(XDpRxSs *InstancePtr)
{
	XDp_SbMsgLinkAddressReplyPortDetail Port;
	u8 PortIndex;
	u8 GuidIndex;
	u8 StreamIndex;

	/* Check for MST */
	if (InstancePtr->Config.MstSupport) {
		/* Ensure that all ports are not exposed in the link address */
		for (PortIndex = 0; PortIndex < XDP_MAX_NPORTS; PortIndex++) {
			XDp_RxMstExposePort(InstancePtr->DpPtr, PortIndex, 0);
		}

		/* Configure the commonality between the downstream sink
		 * devices
		 */
		Port.InputPort = 0;
		Port.PeerDeviceType = 0x3;
		Port.MsgCapStatus = 0;
		Port.DpDevPlugStatus = 1;
		Port.LegacyDevPlugStatus = 0;
		Port.DpcdRev = 0x11;
		Port.NumSdpStreams = InstancePtr->Config.MaxNumAudioCh;
		Port.NumSdpStreamSinks = InstancePtr->Config.MaxNumAudioCh;

		/* Configure the unique port number and GUID for all possible
		 * downstream sinks
		 */
		for (PortIndex = 1; PortIndex < XDPRXSS_MAX_NPORTS;
								PortIndex++) {
			/* Set the GUID to a repeating pattern of the port
			 * number
			 */
			for (GuidIndex = 0; GuidIndex < XDPRXSS_GUID_NBYTES;
								GuidIndex++) {
				Port.Guid[GuidIndex] = PortIndex;
			}
			/* Port.PortNum is set to the index. */
			XDp_RxMstSetPort(InstancePtr->DpPtr, PortIndex, &Port);
		}

		/* Configure each port with I2C, DPCD and PBN values */
		for (StreamIndex = 0;
			StreamIndex < InstancePtr->Config.NumMstStreams;
							StreamIndex++) {
			/* Set I2C maps. */
			if (!InstancePtr->EdidSize[StreamIndex]) {
				XDp_RxSetIicMapEntry(InstancePtr->DpPtr,
					StreamIndex + 1, EDID_IIC_ADDRESS,
					sizeof(GenEdid), GenEdid);
			} else {
				XDp_RxSetIicMapEntry(InstancePtr->DpPtr,
					StreamIndex + 1, EDID_IIC_ADDRESS,
					InstancePtr->EdidSize[StreamIndex],
					InstancePtr->EdidDataPtr[StreamIndex]);
			}

			/* Set DPCD maps. */
			XDp_RxSetDpcdMap(InstancePtr->DpPtr, StreamIndex + 1,
				0, sizeof(GenDpcd), GenDpcd);

			/* Set available PBN. */
			XDp_RxMstSetPbn(InstancePtr->DpPtr, StreamIndex + 1,
				2560);

			/* Expose the ports configured above. Used when
			 * replying to a LINK_ADDRESS request. Make sure that
			 * the number of downstream ports matches the number
			 * exposed, otherwise the LINK_ADDRESS reply will be
			 * incorrect
			 */
			XDp_RxMstExposePort(InstancePtr->DpPtr,
				StreamIndex + 1, 1);
		}

		/* Set up the input port and expose it */
		XDp_RxMstSetInputPort(InstancePtr->DpPtr, 0, NULL);
	}
}

/*****************************************************************************/
/**
*
* This routine is a stub for the asynchronous training pattern 1 interrupt
* callback. On initialization, training pattern 1 interrupt handler is set to
* this callback. It is considered as an training pattern 1 for this handler
* to be invoked.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubTp1Callback(void *InstancePtr)
{
	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;

	/* Verify argument.*/
	Xil_AssertVoid(DpRxSsPtr != NULL);

	if (DpRxSsPtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4) {
		DpRxSsPtr->ltState = 1;
		DpRxSsPtr->ceItrCounter = 0;
	}

	if (MCDP6000_IC_Rev == 0x3200) {
		XDpRxSs_MCDP6000_ResetCrPath (DpRxSsPtr, XDPRXSS_MCDP6000_IIC_SLAVE);
	}

	/* Read link rate */
	DpRxSsPtr->UsrOpt.LinkRate =
		XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
			XDPRXSS_DPCD_LINK_BW_SET);

	/* Read lane count */
	DpRxSsPtr->UsrOpt.LaneCount =
		XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
			XDPRXSS_DPCD_LANE_COUNT_SET);

	if (MCDP6000_IC_Rev == 0x2100) {
		XDpRxSs_MCDP6000_AccessLaneSet(DpRxSsPtr,
				XDPRXSS_MCDP6000_IIC_SLAVE);
	}

	/* Link bandwidth callback */
	if (DpRxSsPtr->LinkBwCallback) {
		if (DpRxSsPtr->Config.IncludeClkWiz)
			XDpRxSs_Set_Dec_Clk(DpRxSsPtr);

		DpRxSsPtr->LinkBwCallback(DpRxSsPtr->LinkBwRef);

		if (DpRxSsPtr->Config.IncludeClkWiz)
			if (XDpRxSs_Get_Dec_Clk_Lock(DpRxSsPtr) == XST_FAILURE)
				xil_printf("Rx Dec clock failed to lock\n\r");
	}

	XDpRxSs_WriteReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
		XDPRXSS_RX_PHY_CONFIG, 0x3800000);

	/* PLL reset callback */
	if (DpRxSsPtr->PllResetCallback) {
		DpRxSsPtr->PllResetCallback(DpRxSsPtr->PllResetRef);
	}

	/* Set vertical blank */
	DpRxSsPtr->VBlankEnable = 1;

	/* Enable vertical blank interrupt */
	XDp_RxInterruptEnable(DpRxSsPtr->DpPtr, XDPRXSS_INTR_VBLANK_MASK);

	/* Set vertical blank count */
	DpRxSsPtr->VBlankCount = 0;
	if (DpRxSsPtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4) {
		DpRxSsPtr->prevLinkRate =
			XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
						XDPRXSS_DPCD_LINK_BW_SET);
		DpRxSsPtr->prevLaneCounts =
			XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
						XDPRXSS_DPCD_LANE_COUNT_SET);
	}
}

/*****************************************************************************/
/**
*
* This routine is a stub for the asynchronous training pattern 2 interrupt
* callback. On initialization, training pattern 2 interrupt handler is set to
* this callback. It is considered as an training pattern 2 for this handler
* to be invoked.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubTp2Callback(void *InstancePtr)
{
	/* Verify argument.*/
	Xil_AssertVoid(InstancePtr != NULL);

	u8 Index;
	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;

	/*Reset HDCP FIFOs*/
	for(Index = 0; Index < 2; Index++) {
		XDp_WriteReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
				XDP_RX_SOFT_RESET,
				XDP_RX_SOFT_RESET_HDCP_MASK);
		XDp_WriteReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
				XDP_RX_SOFT_RESET, 0);
	}

	if (DpRxSsPtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4) {
		DpRxSsPtr->ltState = 2;
		return;
	}
}

/*****************************************************************************/
/**
*
* This routine is a stub for the asynchronous unplug interrupt callback.
* On initialization, unplug interrupt handler is set to this callback. It is
* considered as an unplug for this handler to be invoked.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubUnplugCallback(void *InstancePtr)
{
	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;

	/* Verify argument.*/
	Xil_AssertVoid(DpRxSsPtr != NULL);

	if (DpRxSsPtr->DpPtr->Config.DpProtocol == XDP_PROTOCOL_DP_1_4) {
		DpRxSsPtr->ltState = 0;
		DpRxSsPtr->ceItrCounter = 0;
		DpRxSsPtr->prevLinkRate = 0;
		DpRxSsPtr->prevLaneCounts = 0;
	}

	if (MCDP6000_IC_Rev == 0x2100) {
		XDpRxSs_MCDP6000_ResetDpPath(DpRxSsPtr,
				XDPRXSS_MCDP6000_IIC_SLAVE);
	}

	XDpRxSs_MCDP6000_ModifyRegister(DpRxSsPtr,
			XDPRXSS_MCDP6000_IIC_SLAVE, 0x0A00,
			0x55000000, 0x55000000);

	/* Disable unplug interrupt so that no unplug event when RX is
	 * disconnected
	 */
	XDp_RxInterruptDisable(DpRxSsPtr->DpPtr,
				XDP_RX_INTERRUPT_MASK_UNPLUG_MASK);

	/* Generate a HPD interrupt. Bring down HPD signal for 750us */
	XDp_RxGenerateHpdInterrupt(DpRxSsPtr->DpPtr, 750);

	/* Unplug event callback */
	if (DpRxSsPtr->UnplugCallback) {
		DpRxSsPtr->UnplugCallback(DpRxSsPtr->UnplugRef);
	}
}

/*****************************************************************************/
/**
*
* This routine is a stub for the asynchronous access lane set
* interrupt callback. On initialization, access lane set interrupt handler
* is set to this callback.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubAccessLaneSetCallback(void *InstancePtr)
{
	/* Verify argument.*/
	Xil_AssertVoid(InstancePtr != NULL);

	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;
	u32 read_val;

	u8 training = XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
				      XDP_RX_DPCD_TRAINING_PATTERN_SET);

	if (DpRxSsPtr->ltState == 2 && training != 1) {

		read_val = XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
					   XDP_RX_DPCD_LANE01_STATUS);
		read_val &= 0x0000FF00;

		if (MCDP6000_IC_Rev==0x2100) {
			if (DpRxSsPtr->ceRequestValue != read_val) {
				XDpRxSs_MCDP6000_AccessLaneSet(DpRxSsPtr,
					       XDPRXSS_MCDP6000_IIC_SLAVE);
			}
		}

		/* Update the value to be used in next round */
		DpRxSsPtr->ceRequestValue =
			(XDpRxSs_ReadReg(DpRxSsPtr->DpPtr->Config.BaseAddr,
					 XDP_RX_DPCD_LANE01_STATUS) &
			 0x0000FF00);
	}
}

/*****************************************************************************/
/**
*
* This routine initializes the MCDP6000 part on the VFMC card used
* for DP 1.4.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XDpRxSs_McDp6000_init(void *InstancePtr)
{
	/* Verify argument.*/
	Xil_AssertVoid(InstancePtr != NULL);

	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;
	XDpRxSs_MCDP6000_DpInit(DpRxSsPtr,
				XDPRXSS_MCDP6000_IIC_SLAVE);

}
#if ((XPAR_XHDCP_NUM_INSTANCES > 0) || (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0))
/*****************************************************************************/
/**
*
* This function resets both HDCP 1.4 and 2.2 protocols. This function also
* disables both HDCP 1.4 and 2.2 protocols.
*
* @param InstancePtr is a pointer to the XDpRxSs instance.
*
* @return
*  - XST_SUCCESS if action was successful
*  - XST_FAILURE if action was not successful
*
* @note   None.
*
******************************************************************************/
static int XDpRxSs_HdcpReset(XDpRxSs *InstancePtr)
{
	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	int Status = XST_SUCCESS;

#if (XPAR_XHDCP_NUM_INSTANCES > 0)
	/* HDCP 1.4 */
	/* Resetting HDCP 1.4 causes the state machine to be enabled, therefore
	 * disable must be called immediately after reset is called
	 */
	if (InstancePtr->Hdcp1xPtr) {
		Status = XHdcp1x_Reset(InstancePtr->Hdcp1xPtr);
		/* This is needed to ensure that the previous command
		 * is executed*/
		XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;

		Status = XHdcp1x_Disable(InstancePtr->Hdcp1xPtr);
		/* This is needed to ensure that the previous command
		 * is executed.*/
		XHdcp1x_Poll(InstancePtr->Hdcp1xPtr);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
	}
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
	/* HDCP 2.2*/
	if (InstancePtr->Hdcp22Ptr) {
		Status = XHdcp22Rx_Dp_Reset(InstancePtr->Hdcp22Ptr);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;

		Status = XHdcp22Rx_Dp_Disable(InstancePtr->Hdcp22Ptr);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
	}
#endif

	return Status;
}

/*****************************************************************************/
/**
*
* This function sets the active HDCP protocol and enables it.
* The protocol can be set to either HDCP 1.4, 2.2, or None.
*
* @param InstancePtr is a pointer to the XDpRxSs instance.
* @param Protocol is the requested content protection scheme of type
*        XDpRxSs_HdcpProtocol.
*
* @return
*  - XST_SUCCESS if action was successful
*  - XST_FAILURE if action was not successful
*
* @note   None.
*
******************************************************************************/
int XDpRxSs_HdcpSetProtocol(XDpRxSs *InstancePtr,
		XDpRxSs_HdcpProtocol Protocol)
{
	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Protocol == XDPRXSS_HDCP_NONE) ||
			(Protocol == XDPRXSS_HDCP_14)   ||
			(Protocol == XDPRXSS_HDCP_22));

	int Status;

	/* Set requested protocol */
	InstancePtr->HdcpProtocol = Protocol;

	/* Reset both protocols */
	Status = XDpRxSs_HdcpReset(InstancePtr);
	if (Status != XST_SUCCESS) {
		InstancePtr->HdcpProtocol = XDPRXSS_HDCP_NONE;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
#endif

#if (XPAR_XHDCP22_RX_DP_NUM_INSTANCES > 0)
/*****************************************************************************/
/**
 *
 * This function sets pointers to the HDCP 2.2 keys.
 *
 * @param InstancePtr is a pointer to the XDpRxSs instance.
 * @param KeyType is the type of the key that is being set.
 * @param KeyPtr is the pointer to the key buffer
 *
 * @return None.
 *
 * @note   None.
 *
 ******************************************************************************/
void XDpRxSs_Hdcp22SetKey(XDpRxSs *InstancePtr,
		XDpRxSs_Hdcp22KeyType KeyType, u8 *KeyPtr)
{
	/* Verify argument. */
	Xil_AssertVoid(InstancePtr);
	Xil_AssertVoid((KeyType == XDPRXSS_KEY_HDCP22_LC128) ||
			(KeyType == XDPRXSS_KEY_HDCP22_PRIVATE))
	switch (KeyType) {
		/* HDCP 2.2 LC128 */
		case XDPRXSS_KEY_HDCP22_LC128 :
			InstancePtr->Hdcp22Lc128Ptr = KeyPtr;
			break;
		/* HDCP 2.2 Private key */
		case XDPRXSS_KEY_HDCP22_PRIVATE :
			InstancePtr->Hdcp22PrivateKeyPtr = KeyPtr;
			break;
		default :
			break;
	}
}
#endif

/*****************************************************************************/
/**
 *
 * This function sets Adaptive-Sync capabilities to DisplayPort
 * RX Subsystem
 *
 * @param InstancePtr is a pointer to the XDpRxSs instance.
 * @param Enable is to enable/disable the Adaptive-Sync
 *        capabilities in DisplayPort Rx Subsystem
 *
 * @return XST_SUCCESS
 *
 * @note   None.
 *
 ******************************************************************************/
void XDpRxSs_SetAdaptiveSyncCaps(XDpRxSs *InstancePtr, u32 Enable)
{
	u32 RegVal;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr);
	RegVal = XDpRxSs_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
				 XDP_RX_DTG_ENABLE);
	if (Enable) {
		RegVal |= XDP_RX_ADAPTIVESYNC_SDP_SUPPORTED_MASK |
				XDP_RX_MSA_TIMINGPAR_IGNORED_MASK;
	} else {
		RegVal &= ~(XDP_RX_ADAPTIVESYNC_SDP_SUPPORTED_MASK |
				XDP_RX_MSA_TIMINGPAR_IGNORED_MASK);
	}

	XDpRxSs_WriteReg(InstancePtr->DpPtr->Config.BaseAddr,
			 XDP_RX_DTG_ENABLE, RegVal);
}

/*****************************************************************************/
/**
 *
 * This function masks the Adaptive-Sync interrupts
 * from DisplayPort RX Subsystem
 *
 * @param InstancePtr is a pointer to the XDpRxSs instance.
 * @param Interrupts to mask
 *
 * @note   None.
 *
 ******************************************************************************/
void XDpRxSs_MaskAdaptiveIntr(XDpRxSs *InstancePtr, u32 Mask)
{
	u32 RegVal;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr);
	RegVal = XDpRxSs_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
				 XDP_RX_INTERRUPT_MASK_2);
	RegVal |= (Mask & XDP_RX_ADAPTIVESYNC_SDP_VBLANK_INTERRUPT_STREAMX_MASK);
	XDpRxSs_WriteReg(InstancePtr->DpPtr->Config.BaseAddr,
			 XDP_RX_INTERRUPT_MASK_2, RegVal);
}

/*****************************************************************************/
/**
 *
 * This function unmasks Adaptive-Sync interrupt
 * from DisplayPort RX Subsystem
 *
 * @param InstancePtr is a pointer to the XDpRxSs instance.
 * @param Interrupts to unmask
 *
 * @note   None.
 *
 ******************************************************************************/
void XDpRxSs_UnMaskAdaptiveIntr(XDpRxSs *InstancePtr, u32 Mask)
{
	u32 RegVal;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr);
	RegVal = XDpRxSs_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
				 XDP_RX_INTERRUPT_MASK_2);
	RegVal &= (~Mask);
	XDpRxSs_WriteReg(InstancePtr->DpPtr->Config.BaseAddr,
			 XDP_RX_INTERRUPT_MASK_2, RegVal);
}

/*****************************************************************************/
/**
*
* This function retrieves the current vblank value of
* the incoming video stream.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @Stream	Stream is the stream number to get the vblank.
*
* @return
*		- The current vblank value
*
* @note		This function has to be called after
		assertion of Bit-30 of XDP_RX_INTERRUPT_CAUSE_2
		register
*
******************************************************************************/
int XDpRxSs_GetVblank(XDpRxSs *InstancePtr, u8 Stream)
{
	u32 VBlank;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
			(Stream == XDP_TX_STREAM_ID2) ||
			(Stream == XDP_TX_STREAM_ID3) ||
			(Stream == XDP_TX_STREAM_ID4));

	u32 StreamOffset[4] = {XDP_RX_STREAM1_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM2_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM3_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM4_ADAPTIVE_VBLANK_VTOTAL};

	/* Get the current vblank value */
	VBlank = XDpRxSs_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
				 StreamOffset[Stream - XDP_TX_STREAM_ID1]);
	VBlank &= XDP_RX_ADAPTIVE_VBLANK_MASK;

	return VBlank;
}

/*****************************************************************************/
/**
*
* This function retrieves the current VTotal value of
* the incoming video stream.
*
* @param	InstancePtr is a pointer to the XDpRxSs core instance.
* @Stream	Stream is the stream number to get the vtotal.
*
* Note: This function has to be called after
* assertion Bit-30 of XDP_RX_INTERRUPT_CAUSE_2 register
*
* @return
*		- The current VTotal value
*
* @note		This function has to be called after
		assertion of Bit-30 of XDP_RX_INTERRUPT_CAUSE_2
		register
*
******************************************************************************/
int XDpRxSs_GetVtotal(XDpRxSs *InstancePtr, u8 Stream)
{
	u32 VTotal;

	/* Verify arguments.*/
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Stream == XDP_TX_STREAM_ID1) ||
			(Stream == XDP_TX_STREAM_ID2) ||
			(Stream == XDP_TX_STREAM_ID3) ||
			(Stream == XDP_TX_STREAM_ID4));

	u32 StreamOffset[4] = {XDP_RX_STREAM1_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM2_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM3_ADAPTIVE_VBLANK_VTOTAL,
			XDP_RX_STREAM4_ADAPTIVE_VBLANK_VTOTAL};

	/* Get stream map of the stream(s) */
	VTotal = XDpRxSs_ReadReg(InstancePtr->DpPtr->Config.BaseAddr,
				 StreamOffset[Stream - XDP_TX_STREAM_ID1]);
	VTotal = VTotal >> XDP_RX_ADAPTIVE_VTOTAL_SHIFT;

	return VTotal;
}

static void XDpRxSs_Set_Dec_Clk(XDpRxSs *InstancePtr)
{
	u32 HighTime, DivEdge, Reg, P5Enable, P5fEdge;
	u16 Oval, Dval, Mval;

	/*
	 * For each line rate, the M, D, Div values for MMCM are chosen such that
	 * MMCM gives a /20 clock output for /16 clk input.
	 *
	 * GT ch0outclk (/16) --> MMCM --> /20 clock
	 *
	 * Thus:
	 * 8.1G  : Input MMCM clock is 506.25, output is 405
	 * 5.4G  : Input MMCM clock is 337.5, output is 270
	 * 2.7G  : Input MMCM clock is 168.75, output is 135
	 * 1.62G : Input MMCM clock is 101.25, output is 81
	 */
	switch (InstancePtr->UsrOpt.LinkRate) {
	case XDP_LINK_BW_SET_810GBPS:
		Mval = M_VAL_405;
		break;
	case XDP_LINK_BW_SET_540GBPS:
		Mval = M_VAL_270;
		break;
	case XDP_LINK_BW_SET_270GBPS:
		Mval = M_VAL_135;
		break;
	default:
		Mval = M_VAL_81;
	}
	Dval = D_VAL_ALL;
	Oval = Mval / 4;

	/*
	 * MMCM is dynamically programmed for the respective rate
	 * using the M, D, Div values
	 */
	HighTime = (Oval / 4);
	Reg =  XCLK_WIZ_REG3_PREDIV2 | XCLK_WIZ_REG3_USED | XCLK_WIZ_REG3_MX;
	if (Oval % 4 <= 1) {
		DivEdge = 0;
	} else {
		DivEdge = 1;
	}
	Reg |= (DivEdge << 8);
	P5fEdge = Oval % 2;
	P5Enable = Oval % 2;
	Reg = Reg | P5Enable <<
		(XCLK_WIZ_CLKOUT0_P5EN_SHIFT |
		 P5fEdge << XCLK_WIZ_CLKOUT0_P5FEDGE_SHIFT);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG3_OFFSET, Reg);
	Reg = HighTime | HighTime << 8;
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG4_OFFSET, Reg);

	/* Implement D */
	HighTime = (Dval / 2);
	Reg  = 0;
	Reg = Reg & ~(1 << XCLK_WIZ_REG12_EDGE_SHIFT);
	DivEdge = Dval % 2;
	Reg = Reg | DivEdge << XCLK_WIZ_REG12_EDGE_SHIFT;
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG12_OFFSET, Reg);
	Reg = HighTime | HighTime << 8;
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG13_OFFSET, Reg);

	/* Implement M*/
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG25_OFFSET, 0);

	DivEdge = Mval % 2;
	HighTime = Mval / 2;
	Reg = HighTime | HighTime << 8;
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG2_OFFSET, Reg);
	Reg = XCLK_WIZ_REG1_PREDIV2 | XCLK_WIZ_REG1_EN | XCLK_WIZ_REG1_MX;

	if (DivEdge) {
		Reg = Reg | XCLK_WIZ_REG1_EDGE_MASK;
	} else {
		Reg = Reg & ~XCLK_WIZ_REG1_EDGE_MASK;
	}
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_REG1_OFFSET, Reg);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG11_OFFSET,
			XCLK_WIZ_REG11_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG14_OFFSET,
			XCLK_WIZ_REG14_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG15_OFFSET,
			XCLK_WIZ_REG15_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG16_OFFSET,
			XCLK_WIZ_REG16_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG17_OFFSET,
			XCLK_WIZ_REG17_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr, XCLK_WIZ_REG26_OFFSET,
			XCLK_WIZ_REG26_VAL);
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_RECONFIG_OFFSET,
			(XCLK_WIZ_RECONFIG_LOAD | XCLK_WIZ_RECONFIG_SADDR));
}

int XDpRxSs_Get_Dec_Clk_Lock(XDpRxSs *InstancePtr)
{
	u32 retry = 0;
	/* MMCM issued a reset */
	XDpRxSs_WriteReg(InstancePtr->clk_wiz_abs_addr,
			XCLK_WIZ_SWRST_OFFSET, XCLK_WIZ_SWRST_VAL);
	while(!(XDpRxSs_ReadReg(InstancePtr->clk_wiz_abs_addr,
					XCLK_WIZ_STATUS_OFFSET) & 1)) {
		if(retry == XCLK_WIZ_STATUS_RETRY)
			return XST_FAILURE;
		usleep(XCLK_WIZ_STATUS_WAIT);
		retry++;
	}

	return XST_SUCCESS;
}
/** @} */
