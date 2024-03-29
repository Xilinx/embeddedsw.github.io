/*******************************************************************************
* Copyright (C) 2017 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/******************************************************************************/
/**
 *
 * @file xdppsu_poll_example.c
 *
 * Contains a design example using the XDpPsu driver with polling. Once the
 * polling detects a Hot-Plug-Detect event (HPD - DisplayPort cable is plugged/
 * unplugged or the monitor is turned on/off), the main link will be trained.
 *
 * @note	For this example to display output, the user will need to
 *		implement initialization of the system (DpPsu_PlatformInit) and,
 *		after training is complete, implement configuration of the video
 *		stream source in order to provide the DisplayPort core with
 *		input (DpPsu_StreamSrc* - called in xdppsu_common_example.c).
 *
 * @note	The functions DpPsu_PlatformInit and DpPsu_StreamSrc* are declared
 *		extern in xdppsu_common_example.h and are left up to the user to
 *		implement.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 * 1.0   aad  14/10/17 Initial creation.
 * </pre>
 *
*******************************************************************************/

/******************************* Include Files ********************************/

#include "xdppsu_common_example.h"

/**************************** Function Prototypes *****************************/

#ifndef SDT
u32 DpPsu_PollExample(XDpPsu *InstancePtr, u16 DeviceId);
#else
u32 DpPsu_PollExample(XDpPsu *InstancePtr, u32 BaseAddress);
#endif
static void DpPsu_HpdPoll(XDpPsu *InstancePtr);

/**************************** Function Definitions ****************************/

/******************************************************************************/
/**
 * This function is the main function of the XDpPsu polling example.
 *
 * @param	None.
 *
 * @return
 *		- XST_FAILURE if the polling example was unsuccessful - system
 *		  setup failed.
 *
 * @note	Unless setup failed, main will never return since
 *		DpPsu_PollExample is blocking (it is continuously polling for
 *		Hot-Plug-Detect (HPD) events.
 *
*******************************************************************************/
int main(void)
{
    XDpPsu DpPsuInstance;
	/* Run the XDpPsu polling example. */
#ifndef SDT
	DpPsu_PollExample(&DpPsuInstance, DPPSU_DEVICE_ID);
#else
	DpPsu_PollExample(&DpPsuInstance, DPPSU_BASEADDR);
#endif

	return XST_FAILURE;
}

/******************************************************************************/
/**
 * The main entry point for the polling example using the XDpPsu driver. This
 * function will set up the system. If this is successful, this example will
 * begin polling the Hot-Plug-Detect (HPD) status registers for HPD events. Once
 * a connection event or a pulse is detected, link training will commence (if
 * needed) and a video stream will start being sent over the main link.
 *
 * @param	InstancePtr is a pointer to the XDpPsu instance.
 * @param	DeviceId is the unique device ID of the DisplayPort TX core
 *		instance.
 *
 * @return
 *		- XST_FAILURE if the system setup failed.
 *		- XST_SUCCESS should never return since this function, if setup
 *		  was successful, is blocking.
 *
 * @note	If system setup was successful, this function is blocking in
 *		order to illustrate polling taking place for HPD events.
 *
*******************************************************************************/
#ifndef SDT
u32 DpPsu_PollExample(XDpPsu *InstancePtr, u16 DeviceId)
#else
u32 DpPsu_PollExample(XDpPsu *InstancePtr, u32 BaseAddress)
#endif
{
	u32 Status;

	/* Do platform initialization here. This is hardware system specific -
	 * it is up to the user to implement this function. */
	DpPsu_PlatformInit();
	/******************/
#ifndef SDT
	Status = DpPsu_SetupExample(InstancePtr, DeviceId);
#else
	Status = DpPsu_SetupExample(InstancePtr, BaseAddress);
#endif
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Continuously poll for HPD events. */
	while (1) {
		DpPsu_HpdPoll(InstancePtr);
	}

	return XST_SUCCESS;
}

/******************************************************************************/
/**
 * This function polls the XDPPSU_INTERRUPT_SIG_STATE and XDPPSU_INTERRUPT_STATUS
 * registers for Hot-Plug-Detect (HPD) events and handles them accordingly. If a
 * connection or pulse event is detected, link training will begin (if required)
 * and a video stream will be initiated.
 *
 * @param	InstancePtr is a pointer to the XDpPsu instance.
 *
 * @return	None.
 *
 * @note	None.
 *
*******************************************************************************/
static void DpPsu_HpdPoll(XDpPsu *InstancePtr)
{
	u32 InterruptSignalState;
	u32 InterruptStatus;
	u32 HpdState;
	u32 HpdEvent;
	u32 HpdPulseDetected;
	u32 HpdDuration;

	/* Read interrupt registers. */
	InterruptSignalState = XDpPsu_ReadReg(InstancePtr->Config.BaseAddr,
						XDPPSU_INTERRUPT_SIG_STATE);
	InterruptStatus = XDpPsu_ReadReg(InstancePtr->Config.BaseAddr,
							XDPPSU_INTR_STATUS);

	/* Check for HPD events. */
	HpdState = InterruptSignalState &
				XDPPSU_INTERRUPT_SIG_STATE_HPD_STATE_MASK;
	HpdEvent = InterruptStatus & XDPPSU_INTR_HPD_EVENT_MASK;
	HpdPulseDetected = InterruptStatus &
				XDPPSU_INTR_HPD_PULSE_DETECTED_MASK;
	if (HpdPulseDetected) {
		HpdDuration = XDpPsu_ReadReg(InstancePtr->Config.BaseAddr,
							XDPPSU_HPD_DURATION);
	}

	/* HPD event handling. */
	if (HpdState && HpdEvent) {
		xil_printf("+===> HPD connection event detected.\n");

		/* Initiate link training. */
		DpPsu_Run(InstancePtr);
	}
	else if (HpdState && HpdPulseDetected && (HpdDuration >= 250)) {
		xil_printf("===> HPD pulse detected.\n");

		/* Re-train if needed. */
		DpPsu_Run(InstancePtr);
	}
	else if (!HpdState && HpdEvent) {
		xil_printf("+===> HPD disconnection event detected.\n\n");

		/* Disable main link. */
		XDpPsu_EnableMainLink(InstancePtr, 0);
	}
}
