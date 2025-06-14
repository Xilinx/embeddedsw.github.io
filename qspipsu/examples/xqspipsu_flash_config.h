/******************************************************************************
* Copyright (C) 2020 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2025 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xqspipsu_flash_config.h
*
*
* This file contains flash configuration table and flash related defines.
* This file should be included in the example files and compiled along with
* the examples (*.c).
*
* @note
*
* None.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.12  akm 07/07/20 First release
* 1.12	akm 07/07/20 Add support for Macronix flash(MX66U2G45G, MX66L2G45G)
*                    and ISSI flash(IS25LP01G, IS25WP01G) parts.
* 1.13  akm 12/10/20 Set Read command as per the qspi bus width.
* 1.14  akm 07/16/21 Enable Quad Mode for Winbond flashes.
* 1.15  akm 11/19/21 Fix read/write failures on Spansion flash parts.
* 1.17  akm 10/31/22 Add support for Winbond flash w25q02nw.
* 1.18  sb  05/19/23 Update number of sector calculation logic
*           in flash erase API.
* 1.19  sb  11/10/23 Add support for Winbond flash w25q256jw
* 1.19  sb  11/10/23 Add support for ISSI flash is25lp02g
* 1.20  sb  05/20/24 Add support for ISSI flash is25lp01gg
* 1.20  sb  09/13/24 Updated examples to configure correct baud rate value
*
*</pre>
*
 ******************************************************************************/

#ifndef XQSPIPSU_FLASH_CONFIG_H_		/* prevent circular inclusions */
#define XQSPIPSU_FLASH_CONFIG_H_		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xparameters.h"	/* SDK generated parameters */
#include "xqspipsu.h"		/* QSPIPSU device driver */
#include "sleep.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants define the commands which may be sent to the Flash
 * device.
 */
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD		0x02
#define READ_CMD		0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
#define VOLATILE_WRITE_ENABLE_CMD	0x50
#define QUAD_MODE_ENABLE_BIT	0x06
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define READ_ID			0x9F
#define READ_CONFIG_CMD		0x35
#define WRITE_CONFIG_CMD	0x01
#define ENTER_4B_ADDR_MODE	0xB7
#define EXIT_4B_ADDR_MODE	0xE9
#define EXIT_4B_ADDR_MODE_ISSI	0x29
#define READ_ANY_REG_CMD	0x65
#define WRITE_ANY_REG_CMD	0x71
/* 4-byte address opcodes */
#define READ_CMD_4B			0x13
#define FAST_READ_CMD_4B		0x0C
#define DUAL_READ_CMD_4B		0x3C
#define QUAD_READ_CMD_4B		0x6C
#define WRITE_CMD_4B			0x12
#define SEC_ERASE_CMD_4B		0xDC

#define BANK_REG_RD		0x16
#define BANK_REG_WR		0x17
/* Bank register is called Extended Address Register in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5
#define	DIE_ERASE_CMD		0xC4
#define READ_FLAG_STATUS_CMD	0x70

#define WRITE_STATUS_REG_2_CMD	0x31
#define READ_STATUS_REG_2_CMD	0x35
#define WB_QUAD_MODE_ENABLE_BIT	0x01

/*
 * The following constants define the offsets within a FlashBuffer data
 * type for each kind of data.  Note that the read data offset is not the
 * same as the write data because the QSPIPSU driver is designed to allow full
 * duplex transfers such that the number of bytes received is the number
 * sent and received.
 */
#define COMMAND_OFFSET		0 /* Flash instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define ADDRESS_4_OFFSET	4 /* LSB byte of address to read or write
				   * when 4 byte address
				   */
#define MODE_BITS_OFFSET	5 /* Mode bits (CYPRESS_ID_BYTE0 only)*/
#define DATA_OFFSET		5 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				   * reads
				   */
#define DUMMY_SIZE		1 /* Number of dummy bytes for fast, dual and
				   * quad reads
				   */
#define DUMMY_CLOCKS		8 /* Number of dummy bytes for fast, dual and
				   * quad reads
				   */
#define RD_ID_SIZE		4 /* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE		1 /* Bulk Erase command size */
#define SEC_ERASE_SIZE		4 /* Sector Erase command + Sector address */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank
				   * value
				   */
#define RD_CFG_SIZE		2 /* 1 byte Configuration register + RD CFG
				   * command
				   */
#define WR_CFG_SIZE		3 /* WRR command + 1 byte each Status and
				   * Config Reg
				   */
#define DIE_ERASE_SIZE	4	/* Die Erase command + Die address */

/*
 * The following constants specify the extra bytes which are sent to the
 * Flash on the QSPIPSu interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

/*
 * Base address of Flash1
 */
#define FLASH1BASE 0x0000000

/*
 * Sixteen MB
 */
#define SIXTEENMB 0x1000000

/*
 * Die size 128 MB
 */
#define DIE_SIZE_128MB 0x8000000

/*
 * status/configuration register volatile address offset
 */
#define REG_SR1V_ADDR_OFFSET 0x00800000
#define REG_CR1V_ADDR_OFFSET 0x00800002

/*
 * status/configuration register nonvolatile address offset
 */
#define REG_SR1N_ADDR_OFFSET 0x00000000
#define REG_CR1N_ADDR_OFFSET 0x00000002

/*
 * Mask for quad enable bit in Flash configuration register
 */
#define FLASH_QUAD_EN_MASK 0x02

#define FLASH_SRWD_MASK 0x80

/*
 * Bank mask
 */
#define BANKMASK 0xF000000

/*
 * Bus width
 */
#define BUSWIDTH_SINGLE	0
#define BUSWIDTH_DOUBLE	1

/*
 * Identification of Flash
 * Micron:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is first byte of Device ID - 0xBB or 0xBA
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 * Spansion:
 * Byte 0 is Manufacturer ID;
 * Byte 1 is Device ID - Memory Interface type - 0x20 or 0x02
 * Byte 2 is second byte of Device ID describes flash size:
 * 128Mbit : 0x18; 256Mbit : 0x19; 512Mbit : 0x20
 */
#define MICRON_ID_BYTE0		0x20
#define SPANSION_ID_BYTE0	0x01
#define CYPRESS_ID_BYTE0	0x34
#define WINBOND_ID_BYTE0	0xEF
#define MACRONIX_ID_BYTE0	0xC2
#define ISSI_ID_BYTE0		0x9D

/**< Maximum delay count */
#define MAX_DELAY_CNT	1000000000U

/**************************** Type Definitions *******************************/

typedef struct {
	u32 jedec_id;	/* JEDEC ID */

	u32 SectSize;		/* Individual sector size or combined sector
				 * size in case of parallel config
				 */
	u32 NumSect;		/* Total no. of sectors in one/two
				 * flash devices
				 */
	u32 PageSize;		/* Individual page size or
				 * combined page size in case of parallel
				 * config
				 */
	u32 NumPage;		/* Total no. of pages in one/two flash
				 * devices
				 */
	u32 FlashDeviceSize;	/* This is the size of one flash device
				 * NOT the combination of both devices,
				 * if present
				 */
	u8 NumDie;		/* No. of die forming a single flash */
} FlashInfo;

/************************** Variable Definitions *****************************/
FlashInfo Flash_Config_Table[] = {
	/* Spansion */
	/*s25fl064l*/
	{
		0x016017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 1
	},
	/*s25fl128l*/
	{
		0x016018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*s25fl256l*/
	{
		0x016019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*s25fl512s*/
	{
		0x010220, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/* Spansion 1Gbit is handled as 512Mbit stacked */
	/*s25hs512t*/
	{
		0x342B1A, SECTOR_SIZE_256K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/*s25hs01gt*/
	{
		0x342B1B, SECTOR_SIZE_256K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*s25hs02gt*/
	{
		0x342B1C, SECTOR_SIZE_256K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 2
	},
	/*s25hl512t*/
	{
		0x342A1A, SECTOR_SIZE_256K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/*s25hl01gt*/
	{
		0x342A1B, SECTOR_SIZE_256K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*s25hl02gt*/
	{
		0x342A1C, SECTOR_SIZE_256K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 2
	},
	/* Micron */
	/*n25q128a11*/
	{
		0x20bb18, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*n25q128a13*/
	{
		0x20ba18, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*n25q256ax1*/
	{
		0x20bb19, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*n25q256a*/
	{
		0x20ba19, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*mt25qu512a*/
	{
		0x20bb20, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/*n25q512ax3*/
	{
		0x20ba20, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 2
	},
	/*n25q00a*/
	{
		0x20bb21, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 4
	},
	/*n25q00*/
	{
		0x20ba21, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 4
	},
	/*mt25qu02g*/
	{
		0x20bb22, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 4
	},
	/*mt25ql02g*/
	{
		0x20ba22, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 4
	},
	/* Winbond */
	/*w25q128fw*/
	{
		0xef6018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*w25q128jv*/
	{
		0xef7018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*w25q256jw*/
	{
		0xef8019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*w25h02jv*/
	{
		0xef9022, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 4
	},
	/*w25q02nw*/
	{
		0xef8022, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 4
	},
	/* Macronix */
	/*mx66l1g45g*/
	{
		0xc2201b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*mx66l1g55g*/
	{
		0xc2261b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*mx66u1g45g*/
	{
		0xc2253b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*mx66l2g45g*/
	{
		0xc2201c, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 1
	},
	/*mx66u2g45g*/
	{
		0xc2253c, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 1
	},
	/* ISSI */
	/*is25wp080d*/
	{
		0x9d7014, SECTOR_SIZE_64K, NUM_OF_SECTORS16, BYTES256_PER_PAGE,
		0x1000, 0x100000, 1
	},
	/*is25lp080d*/
	{
		0x9d6014, SECTOR_SIZE_64K, NUM_OF_SECTORS16, BYTES256_PER_PAGE,
		0x1000, 0x100000, 1
	},
	/*is25wp016d*/
	{
		0x9d7015, SECTOR_SIZE_64K, NUM_OF_SECTORS32, BYTES256_PER_PAGE,
		0x2000, 0x200000, 1
	},
	/*is25lp016d*/
	{
		0x9d6015, SECTOR_SIZE_64K, NUM_OF_SECTORS32, BYTES256_PER_PAGE,
		0x2000, 0x200000, 1
	},
	/*is25wp032*/
	{
		0x9d7016, SECTOR_SIZE_64K, NUM_OF_SECTORS64, BYTES256_PER_PAGE,
		0x4000, 0x400000, 1
	},
	/*is25lp032*/
	{
		0x9d6016, SECTOR_SIZE_64K, NUM_OF_SECTORS64, BYTES256_PER_PAGE,
		0x4000, 0x400000, 1
	},
	/*is25wp064*/
	{
		0x9d7017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 1
	},
	/*is25lp064*/
	{
		0x9d6017, SECTOR_SIZE_64K, NUM_OF_SECTORS128, BYTES256_PER_PAGE,
		0x8000, 0x800000, 1
	},
	/*is25wp128*/
	{
		0x9d7018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*is25lp128*/
	{
		0x9d6018, SECTOR_SIZE_64K, NUM_OF_SECTORS256, BYTES256_PER_PAGE,
		0x10000, 0x1000000, 1
	},
	/*is25lp256d*/
	{
		0x9d6019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*is25wp256d*/
	{
		0x9d7019, SECTOR_SIZE_64K, NUM_OF_SECTORS512, BYTES256_PER_PAGE,
		0x20000, 0x2000000, 1
	},
	/*is25lp512m*/
	{
		0x9d601a, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/*is25wp512m*/
	{
		0x9d701a, SECTOR_SIZE_64K, NUM_OF_SECTORS1024, BYTES256_PER_PAGE,
		0x40000, 0x4000000, 1
	},
	/*is25lp01g*/
	{
		0x9d601b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*is25lp01gg*/
	{
		0x9d6021, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*is25wp01g*/
	{
		0x9d701b, SECTOR_SIZE_64K, NUM_OF_SECTORS2048, BYTES256_PER_PAGE,
		0x80000, 0x8000000, 1
	},
	/*is25lp02g*/
	{
		0x9d6022, SECTOR_SIZE_64K, NUM_OF_SECTORS4096, BYTES256_PER_PAGE,
		0x100000, 0x10000000, 1
	}
};

static INLINE u32 CalculateFCTIndex(u32 ReadId, u32 *FCTIndex)
{
	u32 Index;

	for (Index = 0; Index < sizeof(Flash_Config_Table) / sizeof(Flash_Config_Table[0]);
	     Index++) {
		if (ReadId == Flash_Config_Table[Index].jedec_id) {
			*FCTIndex = Index;
			return XST_SUCCESS;
		}
	}

	return XST_FAILURE;
}

static INLINE u8 CalculatePreScaler(u8 BaudRateDiv)
{
	u8 PreScaler = 0;
	while(BaudRateDiv > 2){
		BaudRateDiv >>= 1;
		PreScaler++;
	}

	return PreScaler;
}

#ifdef __cplusplus
}
#endif

#endif /* XQSPIPSU_FLASH_CONFIG_H_ */
/** @} */
