/*******************************************************************
* Copyright (C) 2010-2015 Xilinx, Inc. All rights reserved.
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

*******************************************************************************/


#include "xparameters.h"
#include "xdualsplitter.h"

/*
* The configuration table for devices
*/

XDualSplitter_Config XDualSplitter_ConfigTable[] =
{
	{
#ifdef XPAR_XDUALSPLITTER_NUM_INSTANCES
#if XPAR_XDUALSPLITTER_NUM_INSTANCES > 0
		XPAR_DUALSPLITTER_0_DEVICE_ID,
		XPAR_DUALSPLITTER_0_BASEADDR,
		XPAR_DUALSPLITTER_0_ACTIVE_COLS,
		XPAR_DUALSPLITTER_0_ACTIVE_ROWS,
		XPAR_DUALSPLITTER_0_MAX_SEGMENTS,
		XPAR_DUALSPLITTER_0_AXIS_VIDEO_MAX_TDATA_WIDTH,
		XPAR_DUALSPLITTER_0_AXIS_VIDEO_MAX_ITDATASMPLS_PER_CLK,
		XPAR_DUALSPLITTER_0_AXIS_VIDEO_MAX_OTDATASMPLS_PER_CLK,
		XPAR_DUALSPLITTER_0_MAX_OVRLAP,
		XPAR_DUALSPLITTER_0_MAX_SMPL_WIDTH,
		XPAR_DUALSPLITTER_0_HAS_AXI4_LITE,
		XPAR_DUALSPLITTER_0_HAS_IRQ
#endif
#endif
	}
};
