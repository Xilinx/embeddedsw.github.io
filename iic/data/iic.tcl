###############################################################################
#
# Copyright (C) 2004 - 2014 Xilinx, Inc.  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#
#
#
# MODIFICATION HISTORY:
# Ver      Who    Date     Changes
# -------- ------ -------- ------------------------------------
# 3.0     adk    12/10/13 Updated as per the New Tcl API's
##############################################################################
## @BEGIN_CHANGELOG EDK_M
##  Removed the local ::hsi::utils::define_canonical_xpars API as there is
##  a common API in the tcl of the tools
##
## @END_CHANGELOG

## @BEGIN_CHANGELOG EDK_LS3
##   Updated to handle the corner cases described in CR #518193 while
##   generating canonical definitions
##
## @END_CHANGELOG

#uses "xillib.tcl"

proc generate {drv_handle} {
    ::hsi::utils::define_include_file $drv_handle "xparameters.h" "XIic" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" "C_TEN_BIT_ADR" "C_GPO_WIDTH"
    ::hsi::utils::define_config_file $drv_handle "xiic_g.c" "XIic"  "DEVICE_ID" "C_BASEADDR" "C_TEN_BIT_ADR" "C_GPO_WIDTH"

    ::hsi::utils::define_canonical_xpars $drv_handle "xparameters.h" "Iic" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" "C_TEN_BIT_ADR" "C_GPO_WIDTH"
}
