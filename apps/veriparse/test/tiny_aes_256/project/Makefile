CURRENT_DIR      := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
TESTBENCH_MODULE := test_aes_256

TESTBENCH_FILE    = $(CURRENT_DIR)/../src/$(TESTBENCH_MODULE).v

VCD_FILE          = $(TESTBENCH_MODULE).vcd

INCLUDE_DIRS      =

TOP_FILE          = $(CURRENT_DIR)/../src/aes_256.v
TOP_MODULE        = aes_256

TESTBENCH_DEPS    =

TOP_DEPS          = $(CURRENT_DIR)/../src/round.v \
                    $(CURRENT_DIR)/../src/table.v

include $(CURRENT_DIR)/../../common/Makefiles/iverilog.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriflat.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriobf.mk
include $(CURRENT_DIR)/../../common/Makefiles/common.mk
