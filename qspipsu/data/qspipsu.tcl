###############################################################################
# Copyright (C) 2014 - 2022 Xilinx, Inc.  All rights reserved.
# Copyright (c) 2022 - 2024 Advanced Micro Devices, Inc. All Rights Reserved.
# SPDX-License-Identifier: MIT
#
###############################################################################
##############################################################################
#
# Modification History
#
# Ver   Who  Date     Changes
# ----- ---- -------- -----------------------------------------------
# 1.0   hk  08/21/14 First release
#       sk  05/06/15 Imported Bus Width Parameter.
# 1.5	nsk 08/14/17 Added CCI support
# 1.9   mus 07/30/19 Added CCI support for Versal at EL1 NS
# 1.11	sd  27/03/20 Added Clock support
# 1.18  sb  08/01/23 Added support for Feed back clock
#
##############################################################################

#uses "xillib.tcl"
set fbclk_status 2

proc generate {drv_handle} {
    ::hsi::utils::define_zynq_include_file $drv_handle "xparameters.h" "XQspiPsu" "NUM_INSTANCES" "DEVICE_ID" "C_S_AXI_BASEADDR" "C_S_AXI_HIGHADDR" "C_QSPI_CLK_FREQ_HZ" "C_QSPI_MODE" "C_QSPI_BUS_WIDTH" "C_QSPI_BAUD_RATE_DIV"
    generate_cci_params $drv_handle "xparameters.h"

	set clocking [common::get_property CONFIG.clocking [hsi::get_os]]
	set is_zynqmp_fsbl_bsp [common::get_property CONFIG.ZYNQMP_FSBL_BSP [hsi::get_os]]
	set cortexa53proc [hsi::get_cells -hier -filter {IP_NAME=="psu_cortexa53"}]
	set isclocking [check_clocking]

	if { $isclocking == 1 &&  $is_zynqmp_fsbl_bsp != true   &&  [llength $cortexa53proc] > 0 && [string match -nocase $clocking "true"] > 0} {
    ::hsi::utils::define_zynq_config_file $drv_handle "xqspipsu_g.c" "XQspiPsu"  "DEVICE_ID" "C_S_AXI_BASEADDR" "C_QSPI_CLK_FREQ_HZ" "C_QSPI_MODE" "C_QSPI_BUS_WIDTH" "IS_CACHE_COHERENT" "REF_CLK" "QSPI_FBCLK" "C_QSPI_BAUD_RATE_DIV"
	} else {
    ::hsi::utils::define_zynq_config_file $drv_handle "xqspipsu_g.c" "XQspiPsu"  "DEVICE_ID" "C_S_AXI_BASEADDR" "C_QSPI_CLK_FREQ_HZ" "C_QSPI_MODE" "C_QSPI_BUS_WIDTH" "IS_CACHE_COHERENT" "QSPI_FBCLK" "C_QSPI_BAUD_RATE_DIV"
	}
    ::hsi::utils::define_zynq_canonical_xpars $drv_handle "xparameters.h" "XQspiPsu" "DEVICE_ID" "C_S_AXI_BASEADDR" "C_S_AXI_HIGHADDR" "C_QSPI_CLK_FREQ_HZ" "C_QSPI_MODE" "C_QSPI_BUS_WIDTH" "IS_CACHE_COHERENT" "C_QSPI_BAUD_RATE_DIV"
    generate_canonical_fbclk_params $drv_handle "xparameters.h"

}

proc check_clocking { } {
	set sw_proc_handle [hsi::get_sw_processor]
	set slaves [common::get_property   SLAVES [  hsi::get_cells -hier $sw_proc_handle]]
	foreach slave $slaves {
		if {[string compare -nocase "psu_crf_apb" $slave] == 0 } {
			return 1
		}
	}
	return 0
}

proc generate_cci_params {drv_handle file_name} {
	global fbclk_status
	set file_handle [::hsi::utils::open_include_file $file_name]
	# Get all peripherals connected to this driver
	set ips [::hsi::utils::get_common_driver_ips $drv_handle]

	set sw_processor [hsi::get_sw_processor]
	set processor [hsi::get_cells -hier [common::get_property HW_INSTANCE $sw_processor]]
	set processor_type [common::get_property IP_NAME $processor]
	set isclocking [check_clocking]

	foreach ip $ips {
		set cci_enble 0
		set ref_tag 0xff
		set uSuffix "U"
		if {$processor_type == "psu_cortexa53"} {
			set hypervisor [common::get_property CONFIG.hypervisor_guest [hsi::get_os]]
			set ref_tag  "QSPI_REF"
			if {[string match -nocase $hypervisor "true"]} {
				set cci_enble [common::get_property CONFIG.IS_CACHE_COHERENT $ip]
			}
		} elseif {$processor_type == "psv_cortexa72"} {
			set extra_flags [common::get_property CONFIG.extra_compiler_flags [hsi::get_sw_processor]]
			set flagindex [string first {-DARMA72_EL3} $extra_flags 0]
			if {$flagindex == -1} {
				set cci_enble [common::get_property CONFIG.IS_CACHE_COHERENT $ip]
			}
		}
		puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "IS_CACHE_COHERENT"] $cci_enble"
		if { $isclocking == 1 } {
			puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "REF_CLK"] $ref_tag"
		}
		set ipname [get_cells -hier -filter {IP_NAME == "pspmc" || IP_NAME == "pmcps" || IP_NAME == "psx_wizard"}]
		if {[llength $ipname]} {
			set fbclk [get_property CONFIG.PMC_QSPI_FBCLK [get_cells -hier $ipname]]
			if {[regexp "ENABLE 1" $fbclk matched]} {
				puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "QSPI_FBCLK"] 1$uSuffix"
				set fbclk_status 1
			} else {
				puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "QSPI_FBCLK"] 0$uSuffix"
			        set fbclk_status 0
			}
		} else {
			puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "QSPI_FBCLK"] 0$uSuffix"
			set fbclk_status 0
		}
	}
	close $file_handle
}

proc generate_canonical_fbclk_params {drv_handle file_name} {
        global fbclk_status
        set uSuffix "U"
        set file_handle [::hsi::utils::open_include_file $file_name]
        if { $fbclk_status == 1 } {
                puts $file_handle "\#define XPAR_XQSPIPSU_0_QSPI_FBCLK 1$uSuffix"

        } else {
                puts $file_handle "\#define XPAR_XQSPIPSU_0_QSPI_FBCLK 0$uSuffix"
        }
        close $file_handle
}
