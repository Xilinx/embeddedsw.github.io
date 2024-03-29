/******************************************************************************
* Copyright (C) 2016 - 2020 Xilinx, Inc. All rights reserved.
* Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xdsitxss_hw.h
* @addtogroup dsitxss Overview
* @{
*
* This header file contains identifiers and register-level core functions (or
* macros) that can be used to access the Xilinx MIPI DSI Tx Subsystem core.
*
* For more information about the operation of this core see the hardware
* specification and documentation in the higher level driver
* xdsitxss.h file.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver Who Date    Changes
* --- --- ------- -------------------------------------------------------
* 1.0 ram 11/02/16 Initial Release for MIPI DSI TX subsystem
* 1.1 sss 08/17/16 Added 64 bit support
*     sss 08/26/16 Added "Command Queue Vacancy FIFO Full" interrupt support
* </pre>
*
******************************************************************************/
#ifndef XDSITXSS_HW_H_
#define XDSITXSS_HW_H_		/**< Prevent circular inclusions
				by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_io.h"
#include "xdsi_hw.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define XDsiTxSs_In32	Xil_In32
#define XDsiTxSs_Out32	Xil_Out32

/*****************************************************************************/
/**
*
* This function reads a value from a MIPI DSI Tx Subsystem register.
* A 32 bit read is performed. If the component is implemented in a smaller
* width, only the least significant data is read from the register. The most
* significant data will be read as 0.
*
* @param	BaseAddress is the base address of the XDsiTxSs core instance.
* @param	RegOffset is the register offset of the register (defined at
*		the top of this file).
*
* @return	The 32-bit value of the register.
*
* @note		None
*
******************************************************************************/
static inline u32 XDsiTxSs_ReadReg(UINTPTR BaseAddress, u32 RegOffset)
{
	return	XDsiTxSs_In32(BaseAddress + (u32)RegOffset);
}

/*****************************************************************************/
/**
*
* This function writes a value to a MIPI DSI Tx Subsystem register.
* A 32 bit write is performed. If the component is implemented in a smaller
* width, only the least significant data is written.
*
* @param	BaseAddress is the base address of the XDsiTxSs core instance.
* @param	RegOffset is the register offset of the register (defined at
*			the top of this file) to be written.
* @param	Data is the 32-bit value to write into the register.
*
* @return	None.
*
* @note		None
*
******************************************************************************/
static inline void XDsiTxSs_WriteReg(UINTPTR BaseAddress, u32 RegOffset,
								u32 Data)
{
	XDsiTxSs_Out32(BaseAddress + (u32)RegOffset, (u32)Data);
}

/************************** Function Prototypes ******************************/


/************************** Variable Declarations ****************************/

/** MIPI DSI Tx Subsystem core DSI system interrupt handlers
 *  User will register specific interrupts handler and enable
 *  interrupts by below defining macros
 */

#define XDSITXSS_HANDLER_UNSUPPORT_DATATYPE	XDSI_HANDLER_UNSUPPORT_DATATYPE
#define XDSITXSS_HANDLER_PIXELDATA_UNDERRUN	XDSI_HANDLER_PIXELDATA_UNDERRUN
#define XDSITXSS_HANDLER_CMDQ_FIFOFULL		XDSI_HANDLER_CMDQ_FIFOFULL
#define XDSITXSS_HANDLER_OTHERERROR		XDSI_HANDLER_OTHERERROR

#define XDSITXSS_ISR_DATAIDERR_MASK		XDSI_ISR_DATA_ID_ERR_MASK
#define XDSITXSS_ISR_PIXELUNDERRUN_MASK		XDSI_ISR_PXL_UNDR_RUN_MASK
#define XDSITXSS_ISR_CMDQ_FIFO_FULL_MASK	XDSI_ISR_CMDQ_FIFO_FULL_MASK

#define XDSITXSS_IER_DATAIDERR_MASK		XDSI_IER_DATA_ID_ERR_MASK
#define XDSITXSS_IER_PIXELUNDERRUN_MASK		XDSI_IER_PXL_UNDR_RUN_MASK
#define XDSITXSS_IER_CMDQ_FIFO_FULL_MASK	XDSI_IER_CMDQ_FIFO_FULL_MASK
#define XDSITXSS_IER_ALLINTR_MASK		XDSI_IER_ALLINTR_MASK

#ifdef __cplusplus
}
#endif

#endif
/** @} */
