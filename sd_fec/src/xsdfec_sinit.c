/******************************************************************************
*
* Copyright (C) 2016 - 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*
*
******************************************************************************/

#include "xstatus.h"
#include "xparameters.h"
#include "xsdfec.h"

extern XSdFec_Config XSdFec_ConfigTable[];

XSdFec_Config *XSdFecLookupConfig(u16 DeviceId) {
	XSdFec_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSDFEC_NUM_INSTANCES; Index++) {
		if (XSdFec_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XSdFec_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XSdFecInitialize(XSdFec *InstancePtr, u16 DeviceId) {
	XSdFec_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XSdFecLookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XSdFecCfgInitialize(InstancePtr, ConfigPtr);
}


