/******************************************************************************
* Copyright (C) 2017 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
 *
 * @file xdpdma.h
 *
 * This file defines the functions implemented by the DPDMA driver present
 * in the Zynq Ultrascale MP.
 *
 * @note	None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver	Who   Date     Changes
 * ---- ----- -------- ----------------------------------------------------
 * 1.0  aad   04/12/16 Initial release.
 * </pre>
 *
 *****************************************************************************/


#ifndef XDPDMA_H_
/* Prevent circular inclusions by using protection macros. */
#define XDPDMA_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files **********************************/

#include "xdpdma_hw.h"
#include "xvidc.h"
#include "xil_io.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xavbuf.h"
/************************** Constant Definitions ******************************/

/* Alignment for DPDMA Descriptor and Payload */
#define XDPDMA_DESCRIPTOR_ALIGN 256U
/* DPDMA preamble field */
#define XDPDMA_DESCRIPTOR_PREAMBLE 0xA5
/**************************** Type Definitions ********************************/

/**
 *  This typedef describes the DPDMA descriptor structure and its internals
 *  which will be used when fetching data from a nonlive path
 */
typedef struct {
	u32 Control;			/**<	[7:0] Descriptor Preamble
						[8] Enable completion Interrupt
						[9] Enable descriptor update
						[10] Ignore Done
						[11] AXI burst type
						[15:12] AXACHE
						[17:16] AXPROT
						[18] Descriptor mode
						[19] Last Descriptor
						[20] Enable CRC
						[21] Last descriptor frame
						[31:22] Reserved */
	u32 DSCR_ID;			/**<	[15:0] Descriptor ID
						[31:16] Reserved */
	u32 XFER_SIZE;			/**<	Size of transfer in bytes */
	u32 LINE_SIZE_STRIDE;		/**<	[17:0] Horizontal Resolution
						[31:18] Stride */
	u32 LSB_Timestamp;		/**<	LSB of the Timestamp */
	u32 MSB_Timestamp;		/**<	MSB of the Timestamp */
	u32 ADDR_EXT;			/**<	[15:0] Next descriptor
						extenstion
						[31:16] SRC address extemsion */
	u32 NEXT_DESR;			/**<	Address of next descriptor */
	u32 SRC_ADDR;			/**<	Source Address */
	u32 ADDR_EXT_23;		/**<	[15:0] Address extension for SRC
						Address2
						[31:16] Address extension for
						SRC Address 3 */
	u32 ADDR_EXT_45;		/**<	[15:0] Address extension for SRC
						Address4
						[31:16] Address extension for
						SRC Address 5 */
	u32 SRC_ADDR2;			/**<	Source address of 2nd page */
	u32 SRC_ADDR3;			/**<	Source address of 3rd page */
	u32 SRC_ADDR4;			/**<	Source address of 4th page */
	u32 SRC_ADDR5;			/**<	Source address of 5th page */
	u32 CRC;			/**<	Reserved */

#if defined(__GNUC__)
} XDpDma_Descriptor __attribute__ ((aligned(XDPDMA_DESCRIPTOR_ALIGN)));
#elif defined(__ICCARM__)
} XDpDma_Descriptor _Pragma("data_alignment=XDPDMA_DESCRIPTOR_ALIGN");
#endif

/**
 * This typedef contains configuration information for the DPDMA.
 */
typedef struct {
#ifndef SDT
	u16 DeviceId;			/**< Device ID */
#else
	char *Name;
#endif
	u32 BaseAddr;			/**< Base Address */
#ifdef SDT
    u32 IntrId;     /** Bits[11:0] Interrupt-id Bits[15:12]
                   * trigger type and level flags */
    UINTPTR IntrParent;     /** Bit[0] Interrupt parent type Bit[64/32:1]
                   * Parent base address */
#endif
} XDpDma_Config;

/**
 * The following data structure enumerates the types of
 * DPDMA channels
 */
typedef enum {
	VideoChan,
	GraphicsChan,
	AudioChan0,
	AudioChan1,
} XDpDma_ChannelType;

/**
 * This typedef lists the channel status.
 */
typedef enum {
	XDPDMA_DISABLE,
	XDPDMA_ENABLE,
	XDPDMA_IDLE,
	XDPDMA_PAUSE
} XDpDma_ChannelState;

/**
 * This typedef is the information needed to transfer video info.
 */
typedef struct {
	u64 Address;
	u32 Size;
	u32 Stride;
	u32 LineSize;
} XDpDma_FrameBuffer;
/**
 * This typedef is the information needed to transfer audio info.
 */
typedef struct {
	u64 Address;
	u32 Size;
} XDpDma_AudioBuffer;

/**
 * This typedef defines the Video/Graphics Channel attributes.
 */
typedef struct {
	XDpDma_Descriptor Descriptor0;
	XDpDma_Descriptor Descriptor1;
	XDpDma_Descriptor *Current;
} XDpDma_Channel;

/**
 * This typedef defines the Video Channel attributes.
 */
typedef struct {
	XDpDma_Channel Channel[3];
	u8 TriggerStatus;
	u8 AVBufEn;
	XAVBuf_VideoAttribute *VideoInfo;
	XDpDma_FrameBuffer *FrameBuffer[3];
} XDpDma_VideoChannel;

/**
 * This typedef defines the Graphics Channel attributes.
 */
typedef struct {
	XDpDma_Channel Channel;
	u8 TriggerStatus;
	u8 AVBufEn;
	XAVBuf_VideoAttribute *VideoInfo;
	XDpDma_FrameBuffer *FrameBuffer;
} XDpDma_GfxChannel;

/**
 * This typedef defines the Audio Channel attributes.
 */
typedef struct {
	XDpDma_Descriptor Descriptor0, Descriptor1, Descriptor2;
	XDpDma_Descriptor Descriptor3, Descriptor4, Descriptor5;
	XDpDma_Descriptor Descriptor6, Descriptor7;
	XDpDma_Descriptor *Current;
	u8 TriggerStatus;
	XDpDma_AudioBuffer *Buffer;
	u8 Used;
} XDpDma_AudioChannel;
/*************************************************************************/
/**
 * This callback type represents the handler for a DPDMA VSync interrupt.
 *
 * @param	InstancePtr is a pointer to the XDpDma instance.
 *
 * @note	None.
 *
**************************************************************************/
typedef void (*XDpDma_VSyncInterruptHandler)(void *InstancePtr);

/*************************************************************************/
/**
 * This callback type represents the handler for a DPDMA Done interrupt.
 *
 * @param	InstancePtr is a pointer to the XDpDma instance.
 *
 * @note	None.
 *
**************************************************************************/
typedef void (*XDpDma_DoneInterruptHandler)(void *InstancePtr);

/**
 * The XDpDma driver instance data representing the DPDMA operation.
 */
typedef struct {
	XDpDma_Config Config;
	XDpDma_VideoChannel Video;
	XDpDma_GfxChannel Gfx;
	XDpDma_AudioChannel Audio[2];
	XVidC_VideoTiming *Timing;
	u8 QOS;

	XDpDma_VSyncInterruptHandler VSyncHandler;
	void * VSyncInterruptHandler;

	XDpDma_DoneInterruptHandler DoneHandler;
	void * DoneInterruptHandler;

} XDpDma;

void XDpDma_CfgInitialize(XDpDma *InstancePtr, XDpDma_Config *CfgPtr);
#ifndef SDT
XDpDma_Config *XDpDma_LookupConfig(u16 DeviceId);
#else
XDpDma_Config *XDpDma_LookupConfig(u32 BaseAddress);
#endif
int XDpDma_SetChannelState(XDpDma *InstancePtr, XDpDma_ChannelType Channel,
					XDpDma_ChannelState ChannelState);
void XDpDma_SetQOS(XDpDma *InstancePtr, u8 QOS);
void XDpDma_SetupChannel(XDpDma *InstancePtr, XDpDma_ChannelType Channel);
int XDpDma_SetVideoFormat(XDpDma *InstancePtr, XAVBuf_VideoFormat Format);
int XDpDma_SetGraphicsFormat(XDpDma *InstancePtr, XAVBuf_VideoFormat Format);
int XDpDma_Trigger(XDpDma *InstancePtr, XDpDma_ChannelType Channel);
int XDpDma_ReTrigger(XDpDma *InstancePtr, XDpDma_ChannelType Channel);
void XDpDma_InterruptEnable(XDpDma *InstancePtr, u32 Mask);
void XDpDma_InterruptHandler(XDpDma *InstancePtr);
void XDpDma_VSyncHandler(XDpDma *InstancePtr);
void XDpDma_InitVideoDescriptor(XDpDma_Descriptor *CurrDesc,
				XDpDma_FrameBuffer *FrameBuffer);
void  XDpDma_DisplayVideoFrameBuffer(XDpDma *InstancePtr,
				   XDpDma_FrameBuffer *Plane0,
				   XDpDma_FrameBuffer *Plane1,
				   XDpDma_FrameBuffer *Plane2);
void XDpDma_DisplayGfxFrameBuffer(XDpDma *InstancePtr,
				   XDpDma_FrameBuffer *Plane);
void XDpDma_InitAudioDescriptor(XDpDma_AudioChannel *Channel,
			       XDpDma_AudioBuffer *AudioBuffer);
int XDpDma_PlayAudio(XDpDma *InstancePtr, XDpDma_AudioBuffer *Buffer,
		      u8 ChannelNum);

/*************************** Variable Declarations ****************************/

/**
 * A table of configuration structures containing the configuration information
 * for each DisplayPort TX core in the system.
 */
#ifndef SDT
extern XDpDma_Config XDpDma_ConfigTable[XPAR_XDPDMA_NUM_INSTANCES];
#else
extern XDpDma_Config XDpDma_ConfigTable[];
#endif

#ifdef __cplusplus
}
#endif

#endif /* _XDPDMA_H_ */
