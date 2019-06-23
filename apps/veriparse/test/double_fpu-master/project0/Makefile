CURRENT_DIR      := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

VERIFLAT_FLAGS    =

TESTBENCH_MODULE := fpu_tb

TESTBENCH_FILE    = $(CURRENT_DIR)/../src/fpu_TB.v

VCD_FILE          = $(TESTBENCH_MODULE).vcd

INCLUDE_DIRS      = $(CURRENT_DIR)/../src

TOP_MODULE       := fpu
TOP_FILE          = $(CURRENT_DIR)/../src/fpu_double.v

TESTBENCH_DEPS    =
TOP_DEPS          = $(CURRENT_DIR)/../src/fpu_add.v \
                    $(CURRENT_DIR)/../src/fpu_div.v \
                    $(CURRENT_DIR)/../src/fpu_exceptions.v \
                    $(CURRENT_DIR)/../src/fpu_mul.v \
                    $(CURRENT_DIR)/../src/fpu_round.v \
                    $(CURRENT_DIR)/../src/fpu_sub.v

include $(CURRENT_DIR)/../../common/Makefiles/iverilog.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriflat.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriobf.mk
include $(CURRENT_DIR)/../../common/Makefiles/common.mk
