/*******************************************************************
*
* Copyright (C) 2010-2015 Xilinx, Inc. All rights reserved.
* 
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in
*all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
*THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*SOFTWARE.
*
*
*
*******************************************************************/

#include "xparameters.h"
#include "xv_axi4s_remap.h"

/*
* The configuration table for devices
*/

XV_axi4s_remap_Config XV_axi4s_remap_ConfigTable[]
{
#ifdef XPAR_XV_AXI4S_REMAP_NUM_INSTANCES
	{
		XPAR_V_AXI4S_REMAP_0_DEVICE_ID,
		XPAR_V_AXI4S_REMAP_0_S_AXI_CTRL_BASEADDR,
		XPAR_V_AXI4S_REMAP_0_NUM_VIDEO_COMPONENTS,
		XPAR_V_AXI4S_REMAP_0_MAX_COLS,
		XPAR_V_AXI4S_REMAP_0_MAX_ROWS,
		XPAR_V_AXI4S_REMAP_0_IN_SAMPLES_PER_CLOCK,
		XPAR_V_AXI4S_REMAP_0_OUT_SAMPLES_PER_CLOCK,
		XPAR_V_AXI4S_REMAP_0_CONVERT_SAMPLES_PER_CLOCK,
		XPAR_V_AXI4S_REMAP_0_IN_MAX_DATA_WIDTH,
		XPAR_V_AXI4S_REMAP_0_OUT_MAX_DATA_WIDTH,
		XPAR_V_AXI4S_REMAP_0_IN_HDMI_420,
		XPAR_V_AXI4S_REMAP_0_OUT_HDMI_420,
		XPAR_V_AXI4S_REMAP_0_IN_SAMPLE_DROP,
		XPAR_V_AXI4S_REMAP_0_OUT_SAMPLE_REPEAT
	}
#endif
};
