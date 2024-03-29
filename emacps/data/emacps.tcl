###############################################################################
# Copyright (C) 2011 - 2022 Xilinx, Inc.  All rights reserved.
# Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT
#
###############################################################################
##############################################################################
#
# Modification History
#
# Ver   Who  Date     Changes
# ----- ---- -------- -----------------------------------------------
# 1.00a sdm  11/22/11 Created
# 2.0   adk  10/12/13 Updated as per the New Tcl API's
# 2.1   adk  11/08/14 Fixed the CR#811288 when PCS/PMA core is present in the hw
#		      XPAR_GIGE_PCS_PMA_CORE_PRESENT and phy address values
#		      should export to the xparameters.h file.
# 2.1   bss  09/08/14 Fixed CR#820349 to export phy address in xparameters.h
#		      when GMII to RGMII converter is present in hw.
# 2.2   adk  29/10/14 Fixed CR#827686 when PCS/PMA core is configured with
#		      1000BASE-X mode export proper values to the xparameters.h
#		      file.
# 3.4   hk   11/22/16 Update how PCS definitions are exported for
#                     newer version of Xilinx PCS PMA when PHY address is not
#                     a parameter.
# 3.5   hk   08/14/17 Export cache coherency information
# 3.6   hk   09/14/17 Export PL PCS PMA information for ETH1/2/3 as well.
# 3.7   hk   12/01/17 Export TSU clock frequency to xparameters.h
# 3.8   hk   07/19/18 Added canonical property is cache coherency.
# 3.11  sd   02/14/20 Add clock support.
# 3.11	sd   27/03/20 Added hier design fix
# 3.13  nsk  12/11/20 Modified the tcl to not to use the instance names.
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
    ::hsi::utils::define_zynq_include_file $drv_handle "xparameters.h" "XEmacPs" "NUM_INSTANCES" "DEVICE_ID" "C_S_AXI_BASEADDR" "C_S_AXI_HIGHADDR" "C_ENET_CLK_FREQ_HZ" "C_ENET_SLCR_1000Mbps_DIV0" "C_ENET_SLCR_1000Mbps_DIV1" "C_ENET_SLCR_100Mbps_DIV0" "C_ENET_SLCR_100Mbps_DIV1" "C_ENET_SLCR_10Mbps_DIV0" "C_ENET_SLCR_10Mbps_DIV1" "C_ENET_TSU_CLK_FREQ_HZ"
    generate_cci_params $drv_handle "xparameters.h"

	set clocking [common::get_property CONFIG.clocking [hsi::get_os]]
	set is_zynqmp_fsbl_bsp [common::get_property CONFIG.ZYNQMP_FSBL_BSP [hsi::get_os]]
	set cortexa53proc [hsi::get_cells -hier -filter {IP_NAME=="psu_cortexa53"}]
	set isclocking [check_clocking]

	if { $isclocking == 1 &&  $is_zynqmp_fsbl_bsp != true   &&  [llength $cortexa53proc] > 0 && [string match -nocase $clocking "true"] > 0} {

    ::hsi::utils::define_zynq_config_file $drv_handle "xemacps_g.c" "XEmacPs"  "DEVICE_ID" "C_S_AXI_BASEADDR" "IS_CACHE_COHERENT" "REF_CLK" "C_ENET_SLCR_1000Mbps_DIV0" "C_ENET_SLCR_1000Mbps_DIV1" "C_ENET_SLCR_100Mbps_DIV0" "C_ENET_SLCR_100Mbps_DIV1" "C_ENET_SLCR_10Mbps_DIV0" "C_ENET_SLCR_10Mbps_DIV1"
	} else {
    ::hsi::utils::define_zynq_config_file $drv_handle "xemacps_g.c" "XEmacPs"  "DEVICE_ID" "C_S_AXI_BASEADDR" "IS_CACHE_COHERENT" "C_ENET_SLCR_1000Mbps_DIV0" "C_ENET_SLCR_1000Mbps_DIV1" "C_ENET_SLCR_100Mbps_DIV0" "C_ENET_SLCR_100Mbps_DIV1" "C_ENET_SLCR_10Mbps_DIV0" "C_ENET_SLCR_10Mbps_DIV1"
	}

    ::hsi::utils::define_zynq_canonical_xpars $drv_handle "xparameters.h" "XEmacPs" "DEVICE_ID" "C_S_AXI_BASEADDR" "C_S_AXI_HIGHADDR" "C_ENET_CLK_FREQ_HZ" "C_ENET_SLCR_1000Mbps_DIV0" "C_ENET_SLCR_1000Mbps_DIV1" "C_ENET_SLCR_100Mbps_DIV0" "C_ENET_SLCR_100Mbps_DIV1" "C_ENET_SLCR_10Mbps_DIV0" "C_ENET_SLCR_10Mbps_DIV1" "C_ENET_TSU_CLK_FREQ_HZ"

    generate_gmii2rgmii_params $drv_handle "xparameters.h"

    generate_sgmii_params $drv_handle "xparameters.h"

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

proc generate_gmii2rgmii_params {drv_handle file_name} {
	set file_handle [::hsi::utils::open_include_file $file_name]
	set ips [hsi::get_cells -hier  "*"]
	foreach ip $ips {
		set ipname [common::get_property NAME  $ip]
		set periph [common::get_property IP_NAME  $ip]
		if { [string compare -nocase $periph "ps7_ethernet"] == 0} {
			set phya [is_gmii2rgmii_conv_present $ip]
			if { $phya == 0} {
				close $file_handle
				return 0
			}
			puts $file_handle "/* Definition for the MDIO address for the GMII2RGMII converter PL IP*/"

			if { [string compare -nocase $ipname "ps7_ethernet_0"] == 0} {
				puts $file_handle "\#define XPAR_GMII2RGMIICON_0N_ETH0_ADDR $phya"
			}
			if { [string compare -nocase $ipname "ps7_ethernet_1"] == 0} {
				puts $file_handle "\#define XPAR_GMII2RGMIICON_0N_ETH1_ADDR $phya"
			}
			puts $file_handle "\n/******************************************************************/\n"
		}

	}
	close $file_handle
}

proc is_gmii2rgmii_conv_present {slave} {
	set port_value 0
	set phy_addr 0
	set ipconv 0
	set ips [hsi::get_cells -hier "*"]
	set enetipinstance_name [common::get_property NAME  $slave]

	foreach ip $ips {
		set convipname [common::get_property NAME  $ip]
		set periph [common::get_property IP_NAME $ip]
		if { [string compare -nocase $periph "gmii_to_rgmii"] == 0} {
			set ipconv $ip
			break
		}
	}
	if { $ipconv != 0 }  {
		set port_value [hsi::get_pins -of_objects [hsi::get_nets -of_objects [hsi::get_pins -of_objects $ipconv gmii_txd]]]
		if { $port_value != 0 } {
			set tmp [string first "ENET0" $port_value]
			if { $tmp >= 0 } {
				if { [string compare -nocase $enetipinstance_name "ps7_ethernet_0"] == 0} {
					set phyaddr [::hsi::utils::get_param_value $ipconv C_PHYADDR]
					set phy_addr [::hsi::utils::convert_binary_to_decimal $phyaddr]
					if {[llength $phy_addr] == 0} {
						set phy_addr 0
					}
				}
			} else {
				set tmp0 [string first "ENET1" $port_value]
				if { $tmp0 >= 0 } {
					if { [string compare -nocase $enetipinstance_name "ps7_ethernet_1"] == 0} {
						set phyaddr [::hsi::utils::get_param_value $ipconv C_PHYADDR]
						set phy_addr [::hsi::utils::convert_binary_to_decimal $phyaddr]
						if {[llength $phy_addr] == 0} {
							set phy_addr 0
						}
					}
				}
			}
		}
	}
	return $phy_addr
}

proc generate_sgmii_params {drv_handle file_name} {
	set file_handle [::hsi::utils::open_include_file $file_name]
	set ips [hsi::get_cells -hier "*"]
	set pcs_pma false

	foreach ip $ips {
		set periph [common::get_property IP_NAME  $ip]
		if { [string compare -nocase $periph "gig_ethernet_pcs_pma"] == 0} {
				set pcs_pma true
				set PhyStandard [common::get_property CONFIG.Standard $ip]
				set sgmii_param [common::get_property CONFIG.c_is_sgmii $ip]
		}
		# The below function call to get phy address works only with PL PCS PMA IP versions
		# before 2016.3. For all subsequent versions, the phy address is exported as 0 in
		# xparameters but will be scanned and obtained in upper layers.
		set phya [is_gige_pcs_pma_ip_present $ip]
		if {$pcs_pma == false} {
			continue
		}
		if { $PhyStandard == "1000BASEX" } {
			puts $file_handle "/* Definitions related to PCS PMA PL IP*/"
			puts $file_handle "\#define XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT 1"
			puts $file_handle "\#define XPAR_PCSPMA_1000BASEX_PHYADDR $phya"
			puts $file_handle "\n/******************************************************************/\n"
		} else {
			if {$sgmii_param == true} {
				puts $file_handle "/* Definitions related to PCS PMA PL IP*/"
				puts $file_handle "\#define XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT 1"
				puts $file_handle "\#define XPAR_PCSPMA_SGMII_PHYADDR $phya"
				puts $file_handle "\n/******************************************************************/\n"
			}
		}
		break
	}
	close $file_handle
}

proc is_gige_pcs_pma_ip_present {slave} {
	set port_value 0
	set phy_addr 0
	set ipconv 0

	set ips [hsi::get_cells -hier "*"]
	set enetipinstance_name [common::get_property IP_NAME  $slave]

	foreach ip $ips {
		set convipname [common::get_property NAME  $ip]
		set periph [common::get_property IP_NAME $ip]
		if { [string compare -nocase $periph "gig_ethernet_pcs_pma"] == 0} {
			set sgmii_param [common::get_property CONFIG.c_is_sgmii $ip]
			set PhyStandarrd [common::get_property CONFIG.Standard $ip]
			if {$sgmii_param == true || $PhyStandarrd == "1000BASEX"} {
				set ipconv $ip
			}
			break
		}
	}

	if { $ipconv != 0 }  {
		set port_value [hsi::get_pins -of_objects [hsi::get_nets -of_objects [hsi::get_pins -of_objects $ipconv gmii_txd]]]
		if { $port_value != 0 } {
			set tmp [string first "ENET0" $port_value]
			if { $tmp >= 0 } {
				if { [string compare -nocase $enetipinstance_name "ps7_ethernet"] == 0} {
					set phyaddr [::hsi::utils::get_param_value $ipconv C_PHYADDR]
					set phy_addr [::hsi::utils::convert_binary_to_decimal $phyaddr]
					if {[llength $phy_addr] == 0} {
						set phy_addr 0
					}
				}
			} else {
				set tmp0 [string first "ENET1" $port_value]
				if { $tmp0 >= 0 } {
					if { [string compare -nocase $enetipinstance_name "ps7_ethernet"] == 0} {
						set phyaddr [::hsi::utils::get_param_value $ipconv C_PHYADDR]
						set phy_addr [::hsi::utils::convert_binary_to_decimal $phyaddr]
						puts [format "phy_addr %s phyaddr %s" $phy_addr $phyaddr]
						if {[llength $phy_addr] == 0} {
							set phy_addr 0
						}
					}
				}
			}
		}
	}
	return $phy_addr
}

proc generate_cci_params {drv_handle file_name} {
	set device_id 0
	set file_handle [::hsi::utils::open_include_file $file_name]
	# Get all peripherals connected to this driver
	set ips [::hsi::utils::get_common_driver_ips $drv_handle]

	set sw_processor [hsi::get_sw_processor]
	set processor [hsi::get_cells -hier [common::get_property HW_INSTANCE $sw_processor]]
	set processor_type [common::get_property IP_NAME $processor]
	set isclocking [check_clocking]

	foreach ip $ips {
		set is_cc 0
		set ref_tag 0xff
		if {$processor_type == "psu_cortexa53"} {
			set is_xen [common::get_property CONFIG.hypervisor_guest [hsi::get_os]]
			if {$is_xen == "true"} {
				set is_cc [common::get_property CONFIG.IS_CACHE_COHERENT $ip]
			}
			set ipname [common::get_property NAME $ip]
			set pos [string length $ipname]
			set num [ expr {$pos -1} ]
			set index [string index $ipname $num]
			set ref_tag [string toupper [format "GEM%d_REF" $index ]]
		} elseif {$processor_type == "psv_cortexa72"} {
			set extra_flags [common::get_property CONFIG.extra_compiler_flags [hsi::get_sw_processor]]
			set flagindex [string first {-DARMA72_EL3} $extra_flags 0]
			if {$flagindex == -1} {
				set is_cc [common::get_property CONFIG.IS_CACHE_COHERENT $ip]
			}
		}
		puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "IS_CACHE_COHERENT"] $is_cc"
		set canonical_tag [string toupper [format "XEMACPS_%d" $device_id ]]
		set canonical_name [format "XPAR_%s_IS_CACHE_COHERENT" $canonical_tag]
		puts $file_handle "\#define $canonical_name $is_cc"
		if { $isclocking == 1 } {
			puts $file_handle "\#define [::hsi::utils::get_driver_param_name $ip "REF_CLK"] $ref_tag"
		}
		incr device_id
	}
	close $file_handle
}
