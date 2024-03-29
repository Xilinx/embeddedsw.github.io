/******************************************************************************
* Copyright (C) 2017 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xrfdc_sinit.c
* @addtogroup Overview
* @{
*
* The implementation of the XRFdc component's static initialization
* functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ---    -------- -----------------------------------------------
* 1.0   sk     05/16/17 Initial release
* 5.0   mus    08/17/18 Updated XRFdc_LookupConfig to make use of device
*                       tree instead of xrfdc_g.c, to obtain
*                       XRFdc_Config for provided device id.It is being
*                       achieved through "param-list" property in RFDC
*                       device node, it will be having 1:1 mapping with
*                       the XRFdc_Config structure. Said changes
*                       have been done, to remove the xparameters.h
*                       dependency from RFDC Linux user space driver.
* 7.0   cog    05/13/19 Formatting changes.
*       cog    07/25/19 Added new XRFdc_RegisterMetal() API to register
*                       RFDC with Libmetal.
*       cog    08/02/19 Formatting changes.
* 8.0   cog    02/10/20 Updated addtogroup.
* 8.1   cog    06/24/20 Upversion.
* 9.0   cog    11/25/20 Upversion.
* 10.0  cog    11/26/20 Refactor and split files.
* 11.0  cog    05/31/21 Upversion.
* 11.1  cog    11/16/21 Upversion.
*       cog    02/24/22 Fixed memory leak.
* 12.1  cog    07/04/23 Add support for SDT.
*       cog    07/14/23 Fix issues with SDT flow.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xrfdc.h"
#ifdef __BAREMETAL__
#ifndef SDT
#include "xparameters.h"
#endif
#else
#include <dirent.h>
#include <arpa/inet.h>
#endif
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
#ifdef __BAREMETAL__
extern XRFdc_Config XRFdc_ConfigTable[];
#else
static XRFdc_Config *XRFdc_ConfigTablePtr = NULL;
#endif

#ifndef __BAREMETAL__
/*****************************************************************************/
/**
*
* Compare two strings in the reversed order.This function compares only
* the last "Count" number of characters of Str1Ptr and Str2Ptr.
*
* @param    Str1Ptr is base address of first string
* @param    Str2Ptr is base address of second string
* @param    Count is number of last characters  to be compared between
*           Str1Ptr and Str2Ptr
*
* @return
*           0 if last "Count" number of bytes matches between Str1Ptr and
*           Str2Ptr, else difference in unmatched character.
*
*@note     None.
*
******************************************************************************/
static s32 XRFdc_Strrncmp(const char *Str1Ptr, const char *Str2Ptr, size_t Count)
{
	u16 Len1 = strlen(Str1Ptr);
	u16 Len2 = strlen(Str2Ptr);
	u8 Diff;

	for (; Len1 && Len2; Len1--, Len2--) {
		if ((Diff = Str1Ptr[Len1 - 1] - Str2Ptr[Len2 - 1]) != 0) {
			return Diff;
		}
		if (--Count == 0) {
			return 0;
		}
	}

	return (Len1 - Len2);
}

/*****************************************************************************/
/**
*
* Traverse "/sys/bus/platform/device" directory, to find RFDC device entry,
* corresponding to provided device id. If device entry corresponding to said
* device id is found, store it in output buffer DevNamePtr.
*
* @param    DevNamePtr is base address of char array, where device name
*           will be stored
* @param    DevId contains the ID of the device to look up the
*           RFDC device name entry in "/sys/bus/platform/device"
*
* @return
 *           - XRFDC_SUCCESS if successful.
 *           - XRFDC_FAILURE if device entry not found for given device id.
 *
 *@note     None.
*
******************************************************************************/
s32 XRFdc_GetDeviceNameByDeviceId(char *DevNamePtr, u16 DevId)
{
	s32 Status = XRFDC_FAILURE;
	u32 Data = 0;
	char CompatibleString[NAME_MAX];
	struct metal_device *DevicePtr;
	DIR *DirPtr;
	struct dirent *DirentPtr;
	char Len = strlen(XRFDC_COMPATIBLE_STRING);
	char SignLen = strlen(XRFDC_SIGNATURE);

	DirPtr = opendir(XRFDC_PLATFORM_DEVICE_DIR);
	if (DirPtr) {
		while ((DirentPtr = readdir(DirPtr)) != NULL) {
			if (XRFdc_Strrncmp(DirentPtr->d_name, XRFDC_SIGNATURE, SignLen) == 0) {
				Status = metal_device_open("platform", DirentPtr->d_name, &DevicePtr);
				if (Status) {
					metal_log(METAL_LOG_ERROR, "\n Failed to open device %s", DirentPtr->d_name);
					continue;
				}
				Status = metal_linux_get_device_property(DevicePtr, XRFDC_COMPATIBLE_PROPERTY,
									 CompatibleString, Len);
				if (Status < 0) {
					metal_log(METAL_LOG_ERROR, "\n Failed to read device tree property");
				} else if (strncmp(CompatibleString, XRFDC_COMPATIBLE_STRING, Len) == 0) {
					Status = metal_linux_get_device_property(DevicePtr, XRFDC_CONFIG_DATA_PROPERTY,
										 &Data, XRFDC_DEVICE_ID_SIZE);
					if (Status < 0) {
						metal_log(METAL_LOG_ERROR, "\n Failed to read device tree property");
					} else if (Data == DevId) {
						strcpy(DevNamePtr, DirentPtr->d_name);
						Status = XRFDC_SUCCESS;
						metal_device_close(DevicePtr);
						break;
					}
				}
				metal_device_close(DevicePtr);
			}
		}
	}

   Status = (s32)closedir(DirPtr);
   if (Status < 0) {
      metal_log(METAL_LOG_ERROR, "\n Failed to close directory");
   }

	return Status;
}
#endif
/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param    DeviceId contains the ID of the device to look up the
*           configuration for.
*
* @return
*
* A pointer to the configuration found or NULL if the specified device ID was
* not found. See xrfdc.h for the definition of XRFdc_Config.
*
* @note     None.
*
******************************************************************************/
#ifdef __BAREMETAL__
#ifndef SDT
XRFdc_Config *XRFdc_LookupConfig(u16 DeviceId)
#else
XRFdc_Config *XRFdc_LookupConfig(metal_phys_addr_t BaseAddr)
#endif
#else
XRFdc_Config *XRFdc_LookupConfig(u16 DeviceId)
#endif
{
	XRFdc_Config *CfgPtr = NULL;
#ifndef __BAREMETAL__
	s32 Status = 0;
	u32 NumInstances;
	struct metal_device *Deviceptr;
	char DeviceName[NAME_MAX];

	Status = XRFdc_GetDeviceNameByDeviceId(DeviceName, DeviceId);
	if (Status != XRFDC_SUCCESS) {
		metal_log(METAL_LOG_ERROR, "\n Invalid device id %d", DeviceId);
		goto RETURN_PATH2;
	}

	Status = metal_device_open(XRFDC_BUS_NAME, DeviceName, &Deviceptr);
	if (Status) {
		metal_log(METAL_LOG_ERROR, "\n Failed to open device %s", DeviceName);
		goto RETURN_PATH2;
	}

	if (XRFdc_ConfigTablePtr == NULL) {
		Status = metal_linux_get_device_property(Deviceptr, XRFDC_NUM_INSTANCES_PROPERTY, &NumInstances,
							 XRFDC_NUM_INST_SIZE);
		if (Status < 0) {
			metal_log(METAL_LOG_ERROR, "\n Failed to read device tree property %s\r\n",
				  XRFDC_NUM_INSTANCES_PROPERTY);
			goto RETURN_PATH1;
		}
		XRFdc_ConfigTablePtr = (XRFdc_Config *)malloc(ntohl(NumInstances) * XRFDC_CONFIG_DATA_SIZE);
		if (XRFdc_ConfigTablePtr == NULL) {
			metal_log(METAL_LOG_ERROR, "\n Failed to allocate memory for XRFdc_ConfigTablePtr");
			goto RETURN_PATH1;
		}
	}
	Status = metal_linux_get_device_property(Deviceptr, XRFDC_CONFIG_DATA_PROPERTY, &XRFdc_ConfigTablePtr[DeviceId],
						 XRFDC_CONFIG_DATA_SIZE);
	if (Status == XRFDC_SUCCESS) {
		CfgPtr = &XRFdc_ConfigTablePtr[DeviceId];
	} else {
		metal_log(METAL_LOG_ERROR, "\n Failed to read device tree property %s", XRFDC_CONFIG_DATA_PROPERTY);
	}
RETURN_PATH1:
	metal_device_close(Deviceptr);
RETURN_PATH2:
#else
	u32 Index;
#ifndef SDT
	for (Index = 0U; Index < (u32)XPAR_XRFDC_NUM_INSTANCES; Index++) {
		if (XRFdc_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XRFdc_ConfigTable[Index];
			break;
		}
	}
#else
	for (Index = (u32)0x0; XRFdc_ConfigTable[Index].Name != NULL; Index++) {
		if ((XRFdc_ConfigTable[Index].BaseAddr == BaseAddr) || !BaseAddr) {
			CfgPtr = &XRFdc_ConfigTable[Index];
			break;
		}
	}
#endif
#endif
	return (XRFdc_Config *)CfgPtr;
}

/*****************************************************************************/
/**
*
* Register/open the deviceand map RFDC to the IO region.
*
* @param    InstancePtr is a pointer to the XRfdc instance.
* @param    DeviceId contains the ID of the device to register/map
* @param    DevicePtr is a pointer to the metal device.
*
* @return
*           - XRFDC_SUCCESS if successful.
*           - XRFDC_FAILURE if error occurs.
*
* @note     None.
*
******************************************************************************/
u32 XRFdc_RegisterMetal(XRFdc *InstancePtr, u16 DeviceId, struct metal_device **DevicePtr)
{
	s32 Status;
#ifndef __BAREMETAL__
	char DeviceName[100];
#endif

	Xil_AssertNonvoid(InstancePtr != NULL);

#ifdef __BAREMETAL__
	Xil_AssertNonvoid(DevicePtr != NULL);
	Status = metal_register_generic_device(*DevicePtr);
	if (Status != XRFDC_SUCCESS) {
		metal_log(METAL_LOG_ERROR, "\n Failed to register device");
		goto RETURN_PATH;
	}
#ifndef SDT
	Status = metal_device_open(XRFDC_BUS_NAME, XRFDC_DEV_NAME, DevicePtr);
#else
	Status = metal_device_open(XRFDC_BUS_NAME, (*DevicePtr)->name, DevicePtr);
#endif
	if (Status != XRFDC_SUCCESS) {
		metal_log(METAL_LOG_ERROR, "\n Failed to open device usp_rf_data_converter");
		goto RETURN_PATH;
	}
#else
	Status = XRFdc_GetDeviceNameByDeviceId(DeviceName, DeviceId);
	if (Status != XRFDC_SUCCESS) {
		metal_log(METAL_LOG_ERROR, "\n Failed to find rfdc device with device id %d", DeviceId);
		goto RETURN_PATH;
	}
	Status = metal_device_open(XRFDC_BUS_NAME, DeviceName, DevicePtr);
	if (Status != XRFDC_SUCCESS) {
		metal_log(METAL_LOG_ERROR, "\n Failed to open device %s.\n", DeviceName);
		goto RETURN_PATH;
	}
#endif

	/* Map RFDC device IO region */
	InstancePtr->io = metal_device_io_region(*DevicePtr, DeviceId);
	if (InstancePtr->io == NULL) {
		metal_log(METAL_LOG_ERROR, "\n Failed to map RFDC region for %s.\n", (*DevicePtr)->name);
		Status = XRFDC_FAILURE;
		goto RETURN_PATH;
	}
	InstancePtr->device = *DevicePtr;

	Status = XRFDC_SUCCESS;
RETURN_PATH:
	return (u32)Status;
}
/** @} */
