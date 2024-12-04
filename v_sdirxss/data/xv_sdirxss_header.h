/******************************************************************************
* Copyright (C) 2017 - 2020 Xilinx, Inc. All rights reserved.
* Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#ifndef XV_SDIRXSS_HEADER_H		/* prevent circular inclusions */
#define XV_SDIRXSS_HEADER_H		/* by using protection macros */

#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"

#ifndef SDT
u32 SdiRxSs_SelfTestExample(u32 DeviceId);
#else
u32 SdiRxSs_SelfTestExample(UINTPTR Baseaddress);
#endif

#endif /* end of protection macro */
