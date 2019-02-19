CURRENT_DIR      := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

VERIFLAT_FLAGS    = -p {FIFO_WIDTH:}

TESTBENCH_MODULE := dclkfifolut_tb

TESTBENCH_FILE    = $(CURRENT_DIR)/../src/$(TESTBENCH_MODULE).v

VCD_FILE          = $(TESTBENCH_MODULE).vcd

INCLUDE_DIRS      = $(CURRENT_DIR)/../src

TOP_MODULE       := dclkfifolut
TOP_FILE          = $(CURRENT_DIR)/../src/$(TOP_MODULE).v

TESTBENCH_DEPS    =
TOP_DEPS          = $(CURRENT_DIR)/../src/bin2gray.v \
                    $(CURRENT_DIR)/../src/gray2bin.v

include $(CURRENT_DIR)/../../common/Makefiles/iverilog.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriflat.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriobf.mk
include $(CURRENT_DIR)/../../common/Makefiles/common.mk
