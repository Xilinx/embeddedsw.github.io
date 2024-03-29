/******************************************************************************
* Copyright (C) 2015 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xvprocss_router.h
* @addtogroup vprocss Overview
* @{
* @details
*
* This header file contains the video processing engine data flow setup
* routines and helper functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  rco   07/21/15   Initial Release

* </pre>
*
******************************************************************************/
#ifndef XVPROCSS_ROUTER_H__  /* prevent circular inclusions */
#define XVPROCSS_ROUTER_H__  /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include "xvprocss.h"
/************************** Constant Definitions *****************************/

/************************** Function Prototypes ******************************/
int XVprocSs_BuildRoutingTable(XVprocSs *XVprocSsPtr);
void XVprocSs_ProgRouterMux(XVprocSs *XVprocSsPtr);
void XVprocSs_SetupRouterDataFlow(XVprocSs *XVprocSsPtr);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
