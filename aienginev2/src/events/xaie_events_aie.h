/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file xaie_events_aie.h
* @{
*
* Header file containing all events macros for AIE.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- ------   -------- -----------------------------------------------------
* 1.0   Dishita  11/21/2019  Initial creation
* 1.1   Nishad   06/02/2020  Move event files under events directory
* 1.2   Nishad   06/02/2020  Rename file attribute to xaie_events_aie.h
* 1.3   Nishad   06/03/2020  Rename XAIEGBL_<MODULE>_EVENT_* macros to
*			     XAIE_EVENTS_<MODULE>_*
* 1.4   Nishad   06/09/2020  Fix typo in *_MEMORY_* macro
* </pre>
*
******************************************************************************/
#ifndef XAIE_EVENTS_AIE_H
#define XAIE_EVENTS_AIE_H

/*
 * Macro of all the Events of AIE for all modules: Core, Memory of AIE tile
 * MEM tile and PL tile
 */

/* Declaration of events for core module of aie tile */
#define XAIE_EVENTS_CORE_NONE					0
#define XAIE_EVENTS_CORE_TRUE					1
#define XAIE_EVENTS_CORE_GROUP_0				2
#define XAIE_EVENTS_CORE_TIMER_SYNC				3
#define XAIE_EVENTS_CORE_TIMER_VALUE_REACHED			4
#define XAIE_EVENTS_CORE_PERF_CNT_0				5
#define XAIE_EVENTS_CORE_PERF_CNT_1				6
#define XAIE_EVENTS_CORE_PERF_CNT_2				7
#define XAIE_EVENTS_CORE_PERF_CNT_3				8
#define XAIE_EVENTS_CORE_COMBO_EVENT_0				9
#define XAIE_EVENTS_CORE_COMBO_EVENT_1				10
#define XAIE_EVENTS_CORE_COMBO_EVENT_2				11
#define XAIE_EVENTS_CORE_COMBO_EVENT_3				12
#define XAIE_EVENTS_CORE_GROUP_PC_EVENT				15
#define XAIE_EVENTS_CORE_PC_0					16
#define XAIE_EVENTS_CORE_PC_1					17
#define XAIE_EVENTS_CORE_PC_2					18
#define XAIE_EVENTS_CORE_PC_3					19
#define XAIE_EVENTS_CORE_PC_RANGE_0_1				20
#define XAIE_EVENTS_CORE_PC_RANGE_2_3				21
#define XAIE_EVENTS_CORE_GROUP_STALL				22
#define XAIE_EVENTS_CORE_MEMORY_STALL				23
#define XAIE_EVENTS_CORE_STREAM_STALL				24
#define XAIE_EVENTS_CORE_CASCADE_STALL				25
#define XAIE_EVENTS_CORE_LOCK_STALL				26
#define XAIE_EVENTS_CORE_DEBUG_HALTED				27
#define XAIE_EVENTS_CORE_ACTIVE					28
#define XAIE_EVENTS_CORE_DISABLED				29
#define XAIE_EVENTS_CORE_ECC_ERROR_STALL			30
#define XAIE_EVENTS_CORE_ECC_SCRUBBING_STALL			31
#define XAIE_EVENTS_CORE_GROUP_PROGRAM_FLOW			32
#define XAIE_EVENTS_CORE_INSTR_EVENT_0				33
#define XAIE_EVENTS_CORE_INSTR_EVENT_1				34
#define XAIE_EVENTS_CORE_INSTR_CALL				35
#define XAIE_EVENTS_CORE_INSTR_RETURN				36
#define XAIE_EVENTS_CORE_INSTR_VECTOR				37
#define XAIE_EVENTS_CORE_INSTR_LOAD				38
#define XAIE_EVENTS_CORE_INSTR_STORE				39
#define XAIE_EVENTS_CORE_INSTR_STREAM_GET			40
#define XAIE_EVENTS_CORE_INSTR_STREAM_PUT			41
#define XAIE_EVENTS_CORE_INSTR_CASCADE_GET			42
#define XAIE_EVENTS_CORE_INSTR_CASCADE_PUT			43
#define XAIE_EVENTS_CORE_INSTR_LOCK_ACQUIRE_REQ			44
#define XAIE_EVENTS_CORE_INSTR_LOCK_RELEASE_REQ			45
#define XAIE_EVENTS_CORE_GROUP_ERRORS_0				46
#define XAIE_EVENTS_CORE_GROUP_ERRORS_1				47
#define XAIE_EVENTS_CORE_SRS_SATURATE				48
#define XAIE_EVENTS_CORE_UPS_SATURATE				49
#define XAIE_EVENTS_CORE_FP_OVERFLOW				50
#define XAIE_EVENTS_CORE_FP_UNDERFLOW				51
#define XAIE_EVENTS_CORE_FP_INVALID				52
#define XAIE_EVENTS_CORE_FP_DIV_BY_ZERO				53
#define XAIE_EVENTS_CORE_TLAST_IN_WSS_WORDS_0_2			54
#define XAIE_EVENTS_CORE_PM_REG_ACCESS_FAILURE			55
#define XAIE_EVENTS_CORE_STREAM_PKT_PARITY_ERROR		56
#define XAIE_EVENTS_CORE_CONTROL_PKT_ERROR			57
#define XAIE_EVENTS_CORE_AXI_MM_SLAVE_ERROR			58
#define XAIE_EVENTS_CORE_INSTR_DECOMPRSN_ERROR			59
#define XAIE_EVENTS_CORE_DM_ADDRESS_OUT_OF_RANGE		60
#define XAIE_EVENTS_CORE_PM_ECC_ERROR_SCRUB_CORRECTED		61
#define XAIE_EVENTS_CORE_PM_ECC_ERROR_SCRUB_2BIT		62
#define XAIE_EVENTS_CORE_PM_ECC_ERROR_1BIT			63
#define XAIE_EVENTS_CORE_PM_ECC_ERROR_2BIT			64
#define XAIE_EVENTS_CORE_PM_ADDRESS_OUT_OF_RANGE		65
#define XAIE_EVENTS_CORE_DM_ACCESS_TO_UNAVAILABLE		66
#define XAIE_EVENTS_CORE_LOCK_ACCESS_TO_UNAVAILABLE		67
#define XAIE_EVENTS_CORE_INSTR_EVENT_2				68
#define XAIE_EVENTS_CORE_INSTR_EVENT_3				69
#define XAIE_EVENTS_CORE_GROUP_STREAM_SWITCH			73
#define XAIE_EVENTS_CORE_PORT_IDLE_0				74
#define XAIE_EVENTS_CORE_PORT_RUNNING_0				75
#define XAIE_EVENTS_CORE_PORT_STALLED_0				76
#define XAIE_EVENTS_CORE_PORT_TLAST_0				77
#define XAIE_EVENTS_CORE_PORT_IDLE_1				78
#define XAIE_EVENTS_CORE_PORT_RUNNING_1				79
#define XAIE_EVENTS_CORE_PORT_STALLED_1				80
#define XAIE_EVENTS_CORE_PORT_TLAST_1				81
#define XAIE_EVENTS_CORE_PORT_IDLE_2				82
#define XAIE_EVENTS_CORE_PORT_RUNNING_2				83
#define XAIE_EVENTS_CORE_PORT_STALLED_2				84
#define XAIE_EVENTS_CORE_PORT_TLAST_2				85
#define XAIE_EVENTS_CORE_PORT_IDLE_3				86
#define XAIE_EVENTS_CORE_PORT_RUNNING_3				87
#define XAIE_EVENTS_CORE_PORT_STALLED_3				88
#define XAIE_EVENTS_CORE_PORT_TLAST_3				89
#define XAIE_EVENTS_CORE_PORT_IDLE_4				90
#define XAIE_EVENTS_CORE_PORT_RUNNING_4				91
#define XAIE_EVENTS_CORE_PORT_STALLED_4				92
#define XAIE_EVENTS_CORE_PORT_TLAST_4				93
#define XAIE_EVENTS_CORE_PORT_IDLE_5				94
#define XAIE_EVENTS_CORE_PORT_RUNNING_5				95
#define XAIE_EVENTS_CORE_PORT_STALLED_5				96
#define XAIE_EVENTS_CORE_PORT_TLAST_5				97
#define XAIE_EVENTS_CORE_PORT_IDLE_6				98
#define XAIE_EVENTS_CORE_PORT_RUNNING_6				99
#define XAIE_EVENTS_CORE_PORT_STALLED_6				100
#define XAIE_EVENTS_CORE_PORT_TLAST_6				101
#define XAIE_EVENTS_CORE_PORT_IDLE_7				102
#define XAIE_EVENTS_CORE_PORT_RUNNING_7				103
#define XAIE_EVENTS_CORE_PORT_STALLED_7				104
#define XAIE_EVENTS_CORE_PORT_TLAST_7				105
#define XAIE_EVENTS_CORE_GROUP_BROADCAST			106
#define XAIE_EVENTS_CORE_BROADCAST_0				107
#define XAIE_EVENTS_CORE_BROADCAST_1				108
#define XAIE_EVENTS_CORE_BROADCAST_2				109
#define XAIE_EVENTS_CORE_BROADCAST_3				110
#define XAIE_EVENTS_CORE_BROADCAST_4				111
#define XAIE_EVENTS_CORE_BROADCAST_5				112
#define XAIE_EVENTS_CORE_BROADCAST_6				113
#define XAIE_EVENTS_CORE_BROADCAST_7				114
#define XAIE_EVENTS_CORE_BROADCAST_8				115
#define XAIE_EVENTS_CORE_BROADCAST_9				116
#define XAIE_EVENTS_CORE_BROADCAST_10				117
#define XAIE_EVENTS_CORE_BROADCAST_11				118
#define XAIE_EVENTS_CORE_BROADCAST_12				119
#define XAIE_EVENTS_CORE_BROADCAST_13				120
#define XAIE_EVENTS_CORE_BROADCAST_14				121
#define XAIE_EVENTS_CORE_BROADCAST_15				122
#define XAIE_EVENTS_CORE_GROUP_USER_EVENT			123
#define XAIE_EVENTS_CORE_USER_EVENT_0				124
#define XAIE_EVENTS_CORE_USER_EVENT_1				125
#define XAIE_EVENTS_CORE_USER_EVENT_2				126
#define XAIE_EVENTS_CORE_USER_EVENT_3				127

/* Declaration of events for Mem module of aie tile */
#define XAIE_EVENTS_MEM_NONE					0
#define XAIE_EVENTS_MEM_TRUE					1
#define XAIE_EVENTS_MEM_GROUP_0					2
#define XAIE_EVENTS_MEM_TIMER_SYNC				3
#define XAIE_EVENTS_MEM_TIMER_VALUE_REACHED			4
#define XAIE_EVENTS_MEM_PERF_CNT_0				5
#define XAIE_EVENTS_MEM_PERF_CNT_1				6
#define XAIE_EVENTS_MEM_COMBO_EVENT_0				7
#define XAIE_EVENTS_MEM_COMBO_EVENT_1				8
#define XAIE_EVENTS_MEM_COMBO_EVENT_2				9
#define XAIE_EVENTS_MEM_COMBO_EVENT_3				10
#define XAIE_EVENTS_MEM_GROUP_WATCHPOINT			15
#define XAIE_EVENTS_MEM_WATCHPOINT_0				16
#define XAIE_EVENTS_MEM_WATCHPOINT_1				17
#define XAIE_EVENTS_MEM_GROUP_DMA_ACTIVITY			20
#define XAIE_EVENTS_MEM_DMA_S2MM_0_START_BD			21
#define XAIE_EVENTS_MEM_DMA_S2MM_1_START_BD			22
#define XAIE_EVENTS_MEM_DMA_MM2S_0_START_BD			23
#define XAIE_EVENTS_MEM_DMA_MM2S_1_START_BD			24
#define XAIE_EVENTS_MEM_DMA_S2MM_0_FINISHED_BD			25
#define XAIE_EVENTS_MEM_DMA_S2MM_1_FINISHED_BD			26
#define XAIE_EVENTS_MEM_DMA_MM2S_0_FINISHED_BD			27
#define XAIE_EVENTS_MEM_DMA_MM2S_1_FINISHED_BD			28
#define XAIE_EVENTS_MEM_DMA_S2MM_0_GO_TO_IDLE			29
#define XAIE_EVENTS_MEM_DMA_S2MM_1_GO_TO_IDLE			30
#define XAIE_EVENTS_MEM_DMA_MM2S_0_GO_TO_IDLE			31
#define XAIE_EVENTS_MEM_DMA_MM2S_1_GO_TO_IDLE			32
#define XAIE_EVENTS_MEM_DMA_S2MM_0_STALLED_LOCK_ACQUIRE		33
#define XAIE_EVENTS_MEM_DMA_S2MM_1_STALLED_LOCK_ACQUIRE		34
#define XAIE_EVENTS_MEM_DMA_MM2S_0_STALLED_LOCK_ACQUIRE		35
#define XAIE_EVENTS_MEM_DMA_MM2S_1_STALLED_LOCK_ACQUIRE		36
#define XAIE_EVENTS_MEM_DMA_S2MM_0_MEMORY_CONFLICT		37
#define XAIE_EVENTS_MEM_DMA_S2MM_1_MEMORY_CONFLICT		38
#define XAIE_EVENTS_MEM_DMA_MM2S_0_MEMORY_CONFLICT		39
#define XAIE_EVENTS_MEM_DMA_MM2S_1_MEMORY_CONFLICT		40
#define XAIE_EVENTS_MEM_GROUP_LOCK				43
#define XAIE_EVENTS_MEM_LOCK_0_ACQ				44
#define XAIE_EVENTS_MEM_LOCK_0_REL				45
#define XAIE_EVENTS_MEM_LOCK_1_ACQ				46
#define XAIE_EVENTS_MEM_LOCK_1_REL				47
#define XAIE_EVENTS_MEM_LOCK_2_ACQ				48
#define XAIE_EVENTS_MEM_LOCK_2_REL				49
#define XAIE_EVENTS_MEM_LOCK_3_ACQ				50
#define XAIE_EVENTS_MEM_LOCK_3_REL				51
#define XAIE_EVENTS_MEM_LOCK_4_ACQ				52
#define XAIE_EVENTS_MEM_LOCK_4_REL				53
#define XAIE_EVENTS_MEM_LOCK_5_ACQ				54
#define XAIE_EVENTS_MEM_LOCK_5_REL				55
#define XAIE_EVENTS_MEM_LOCK_6_ACQ				56
#define XAIE_EVENTS_MEM_LOCK_6_REL				57
#define XAIE_EVENTS_MEM_LOCK_7_ACQ				58
#define XAIE_EVENTS_MEM_LOCK_7_REL				59
#define XAIE_EVENTS_MEM_LOCK_8_ACQ				60
#define XAIE_EVENTS_MEM_LOCK_8_REL				61
#define XAIE_EVENTS_MEM_LOCK_9_ACQ				62
#define XAIE_EVENTS_MEM_LOCK_9_REL				63
#define XAIE_EVENTS_MEM_LOCK_10_ACQ				64
#define XAIE_EVENTS_MEM_LOCK_10_REL				65
#define XAIE_EVENTS_MEM_LOCK_11_ACQ				66
#define XAIE_EVENTS_MEM_LOCK_11_REL				67
#define XAIE_EVENTS_MEM_LOCK_12_ACQ				68
#define XAIE_EVENTS_MEM_LOCK_12_REL				69
#define XAIE_EVENTS_MEM_LOCK_13_ACQ				70
#define XAIE_EVENTS_MEM_LOCK_13_REL				71
#define XAIE_EVENTS_MEM_LOCK_14_ACQ				72
#define XAIE_EVENTS_MEM_LOCK_14_REL				73
#define XAIE_EVENTS_MEM_LOCK_15_ACQ				74
#define XAIE_EVENTS_MEM_LOCK_15_REL				75
#define XAIE_EVENTS_MEM_GROUP_MEMORY_CONFLICT			76
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_0			77
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_1			78
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_2			79
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_3			80
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_4			81
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_5			82
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_6			83
#define XAIE_EVENTS_MEM_CONFLICT_DM_BANK_7			84
#define XAIE_EVENTS_MEM_GROUP_ERRORS				86
#define XAIE_EVENTS_MEM_DM_ECC_ERROR_SCRUB_CORRECTED		87
#define XAIE_EVENTS_MEM_DM_ECC_ERROR_SCRUB_2BIT			88
#define XAIE_EVENTS_MEM_DM_ECC_ERROR_1BIT			89
#define XAIE_EVENTS_MEM_DM_ECC_ERROR_2BIT			90
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_2			91
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_3			92
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_4			93
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_5			94
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_6			95
#define XAIE_EVENTS_MEM_DM_PARITY_ERROR_BANK_7			96
#define XAIE_EVENTS_MEM_DMA_S2MM_0_ERROR			97
#define XAIE_EVENTS_MEM_DMA_S2MM_1_ERROR			98
#define XAIE_EVENTS_MEM_DMA_MM2S_0_ERROR			99
#define XAIE_EVENTS_MEM_DMA_MM2S_1_ERROR			100
#define XAIE_EVENTS_MEM_GROUP_BROADCAST				106
#define XAIE_EVENTS_MEM_BROADCAST_0				107
#define XAIE_EVENTS_MEM_BROADCAST_1				108
#define XAIE_EVENTS_MEM_BROADCAST_2				109
#define XAIE_EVENTS_MEM_BROADCAST_3				110
#define XAIE_EVENTS_MEM_BROADCAST_4				111
#define XAIE_EVENTS_MEM_BROADCAST_5				112
#define XAIE_EVENTS_MEM_BROADCAST_6				113
#define XAIE_EVENTS_MEM_BROADCAST_7				114
#define XAIE_EVENTS_MEM_BROADCAST_8				115
#define XAIE_EVENTS_MEM_BROADCAST_9				116
#define XAIE_EVENTS_MEM_BROADCAST_10				117
#define XAIE_EVENTS_MEM_BROADCAST_11				118
#define XAIE_EVENTS_MEM_BROADCAST_12				119
#define XAIE_EVENTS_MEM_BROADCAST_13				120
#define XAIE_EVENTS_MEM_BROADCAST_14				121
#define XAIE_EVENTS_MEM_BROADCAST_15				122
#define XAIE_EVENTS_MEM_GROUP_USER_EVENT			123
#define XAIE_EVENTS_MEM_USER_EVENT_0				124
#define XAIE_EVENTS_MEM_USER_EVENT_1				125
#define XAIE_EVENTS_MEM_USER_EVENT_2				126
#define XAIE_EVENTS_MEM_USER_EVENT_3				127

/* Declaration of events for PL/ Noc module */
#define XAIE_EVENTS_PL_NONE					0
#define XAIE_EVENTS_PL_TRUE					1
#define XAIE_EVENTS_PL_GROUP_0					2
#define XAIE_EVENTS_PL_TIMER_SYNC				3
#define XAIE_EVENTS_PL_TIMER_VALUE_REACHED			4
#define XAIE_EVENTS_PL_PERF_CNT_0				5
#define XAIE_EVENTS_PL_PERF_CNT_1				6
#define XAIE_EVENTS_PL_COMBO_EVENT_0				7
#define XAIE_EVENTS_PL_COMBO_EVENT_1				8
#define XAIE_EVENTS_PL_COMBO_EVENT_2				9
#define XAIE_EVENTS_PL_COMBO_EVENT_3				10
#define XAIE_EVENTS_PL_GROUP_DMA_ACTIVITY			11
#define XAIE_EVENTS_PL_DMA_S2MM_0_START_BD			12
#define XAIE_EVENTS_PL_DMA_S2MM_1_START_BD			13
#define XAIE_EVENTS_PL_DMA_MM2S_0_START_BD			14
#define XAIE_EVENTS_PL_DMA_MM2S_1_START_BD			15
#define XAIE_EVENTS_PL_DMA_S2MM_0_FINISHED_BD			16
#define XAIE_EVENTS_PL_DMA_S2MM_1_FINISHED_BD			17
#define XAIE_EVENTS_PL_DMA_MM2S_0_FINISHED_BD			18
#define XAIE_EVENTS_PL_DMA_MM2S_1_FINISHED_BD			19
#define XAIE_EVENTS_PL_DMA_S2MM_0_GO_TO_IDLE			20
#define XAIE_EVENTS_PL_DMA_S2MM_1_GO_TO_IDLE			21
#define XAIE_EVENTS_PL_DMA_MM2S_0_GO_TO_IDLE			22
#define XAIE_EVENTS_PL_DMA_MM2S_1_GO_TO_IDLE			23
#define XAIE_EVENTS_PL_DMA_S2MM_0_STALLED_LOCK_ACQUIRE		24
#define XAIE_EVENTS_PL_DMA_S2MM_1_STALLED_LOCK_ACQUIRE		25
#define XAIE_EVENTS_PL_DMA_MM2S_0_STALLED_LOCK_ACQUIRE		26
#define XAIE_EVENTS_PL_DMA_MM2S_1_STALLED_LOCK_ACQUIRE		27
#define XAIE_EVENTS_PL_GROUP_LOCK				28
#define XAIE_EVENTS_PL_LOCK_0_ACQUIRED				29
#define XAIE_EVENTS_PL_LOCK_0_RELEASED				30
#define XAIE_EVENTS_PL_LOCK_1_ACQUIRED				31
#define XAIE_EVENTS_PL_LOCK_1_RELEASED				32
#define XAIE_EVENTS_PL_LOCK_2_ACQUIRED				33
#define XAIE_EVENTS_PL_LOCK_2_RELEASED				34
#define XAIE_EVENTS_PL_LOCK_3_ACQUIRED				35
#define XAIE_EVENTS_PL_LOCK_3_RELEASED				36
#define XAIE_EVENTS_PL_LOCK_4_ACQUIRED				37
#define XAIE_EVENTS_PL_LOCK_4_RELEASED				38
#define XAIE_EVENTS_PL_LOCK_5_ACQUIRED				39
#define XAIE_EVENTS_PL_LOCK_5_RELEASED				40
#define XAIE_EVENTS_PL_LOCK_6_ACQUIRED				41
#define XAIE_EVENTS_PL_LOCK_6_RELEASED				42
#define XAIE_EVENTS_PL_LOCK_7_ACQUIRED				43
#define XAIE_EVENTS_PL_LOCK_7_RELEASED				44
#define XAIE_EVENTS_PL_LOCK_8_ACQUIRED				45
#define XAIE_EVENTS_PL_LOCK_8_RELEASED				46
#define XAIE_EVENTS_PL_LOCK_9_ACQUIRED				47
#define XAIE_EVENTS_PL_LOCK_9_RELEASED				48
#define XAIE_EVENTS_PL_LOCK_10_ACQUIRED				49
#define XAIE_EVENTS_PL_LOCK_10_RELEASED				50
#define XAIE_EVENTS_PL_LOCK_11_ACQUIRED				51
#define XAIE_EVENTS_PL_LOCK_11_RELEASED				52
#define XAIE_EVENTS_PL_LOCK_12_ACQUIRED				53
#define XAIE_EVENTS_PL_LOCK_12_RELEASED				54
#define XAIE_EVENTS_PL_LOCK_13_ACQUIRED				55
#define XAIE_EVENTS_PL_LOCK_13_RELEASED				56
#define XAIE_EVENTS_PL_LOCK_14_ACQUIRED				57
#define XAIE_EVENTS_PL_LOCK_14_RELEASED				58
#define XAIE_EVENTS_PL_LOCK_15_ACQUIRED				59
#define XAIE_EVENTS_PL_LOCK_15_RELEASED				60
#define XAIE_EVENTS_PL_GROUP_ERRORS				61
#define XAIE_EVENTS_PL_AXI_MM_SLAVE_TILE_ERROR			62
#define XAIE_EVENTS_PL_CONTROL_PKT_ERROR			63
#define XAIE_EVENTS_PL_AXI_MM_DECODE_NSU_ERROR			64
#define XAIE_EVENTS_PL_AXI_MM_SLAVE_NSU_ERROR			65
#define XAIE_EVENTS_PL_AXI_MM_UNSUPPORTED_TRAFFIC		66
#define XAIE_EVENTS_PL_AXI_MM_UNSECURE_ACCESS_IN_SECURE_MODE	67
#define XAIE_EVENTS_PL_AXI_MM_BYTE_STROBE_ERROR			68
#define XAIE_EVENTS_PL_DMA_S2MM_0_ERROR				69
#define XAIE_EVENTS_PL_DMA_S2MM_1_ERROR				70
#define XAIE_EVENTS_PL_DMA_MM2S_0_ERROR				71
#define XAIE_EVENTS_PL_DMA_MM2S_1_ERROR				72
#define XAIE_EVENTS_PL_GROUP_STREAM_SWITCH			73
#define XAIE_EVENTS_PL_PORT_IDLE_0				74
#define XAIE_EVENTS_PL_PORT_RUNNING_0				75
#define XAIE_EVENTS_PL_PORT_STALLED_0				76
#define XAIE_EVENTS_PL_PORT_TLAST_0				77
#define XAIE_EVENTS_PL_PORT_IDLE_1				78
#define XAIE_EVENTS_PL_PORT_RUNNING_1				79
#define XAIE_EVENTS_PL_PORT_STALLED_1				80
#define XAIE_EVENTS_PL_PORT_TLAST_1				81
#define XAIE_EVENTS_PL_PORT_IDLE_2				82
#define XAIE_EVENTS_PL_PORT_RUNNING_2				83
#define XAIE_EVENTS_PL_PORT_STALLED_2				84
#define XAIE_EVENTS_PL_PORT_TLAST_2				85
#define XAIE_EVENTS_PL_PORT_IDLE_3				86
#define XAIE_EVENTS_PL_PORT_RUNNING_3				87
#define XAIE_EVENTS_PL_PORT_STALLED_3				88
#define XAIE_EVENTS_PL_PORT_TLAST_3				89
#define XAIE_EVENTS_PL_PORT_IDLE_4				90
#define XAIE_EVENTS_PL_PORT_RUNNING_4				91
#define XAIE_EVENTS_PL_PORT_STALLED_4				92
#define XAIE_EVENTS_PL_PORT_TLAST_4				93
#define XAIE_EVENTS_PL_PORT_IDLE_5				94
#define XAIE_EVENTS_PL_PORT_RUNNING_5				95
#define XAIE_EVENTS_PL_PORT_STALLED_5				96
#define XAIE_EVENTS_PL_PORT_TLAST_5				97
#define XAIE_EVENTS_PL_PORT_IDLE_6				98
#define XAIE_EVENTS_PL_PORT_RUNNING_6				99
#define XAIE_EVENTS_PL_PORT_STALLED_6				100
#define XAIE_EVENTS_PL_PORT_TLAST_6				101
#define XAIE_EVENTS_PL_PORT_IDLE_7				102
#define XAIE_EVENTS_PL_PORT_RUNNING_7				103
#define XAIE_EVENTS_PL_PORT_STALLED_7				104
#define XAIE_EVENTS_PL_PORT_TLAST_7				105
#define XAIE_EVENTS_PL_GROUP_BROADCAST_A			106
#define XAIE_EVENTS_PL_BROADCAST_A_0				107
#define XAIE_EVENTS_PL_BROADCAST_A_1				108
#define XAIE_EVENTS_PL_BROADCAST_A_2				109
#define XAIE_EVENTS_PL_BROADCAST_A_3				110
#define XAIE_EVENTS_PL_BROADCAST_A_4				111
#define XAIE_EVENTS_PL_BROADCAST_A_5				112
#define XAIE_EVENTS_PL_BROADCAST_A_6				113
#define XAIE_EVENTS_PL_BROADCAST_A_7				114
#define XAIE_EVENTS_PL_BROADCAST_A_8				115
#define XAIE_EVENTS_PL_BROADCAST_A_9				116
#define XAIE_EVENTS_PL_BROADCAST_A_10				117
#define XAIE_EVENTS_PL_BROADCAST_A_11				118
#define XAIE_EVENTS_PL_BROADCAST_A_12				119
#define XAIE_EVENTS_PL_BROADCAST_A_13				120
#define XAIE_EVENTS_PL_BROADCAST_A_14				121
#define XAIE_EVENTS_PL_BROADCAST_A_15				122
#define XAIE_EVENTS_PL_GROUP_USER_EVENT				123
#define XAIE_EVENTS_PL_USER_EVENT_0				124
#define XAIE_EVENTS_PL_USER_EVENT_1				125
#define XAIE_EVENTS_PL_USER_EVENT_2				126
#define XAIE_EVENTS_PL_USER_EVENT_3				127

#endif /* XAIE_EVENTS_AIE_H */
/** @} */
