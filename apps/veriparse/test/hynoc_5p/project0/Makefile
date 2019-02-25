CURRENT_DIR      := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

TESTBENCH_MODULE := hynoc_router_5p_tb5

TESTBENCH_FILE    = $(CURRENT_DIR)/../src/$(TESTBENCH_MODULE).v

VCD_FILE          = $(TESTBENCH_MODULE).vcd

INCLUDE_DIRS      = $(CURRENT_DIR)/../src

TOP_MODULE       := hynoc_router_5p
TOP_FILE          = $(CURRENT_DIR)/../src/$(TOP_MODULE).v

TESTBENCH_DEPS    = $(CURRENT_DIR)/../src/hynoc_stream_writer.v \
                    $(CURRENT_DIR)/../src/hynoc_stream_reader.v \
                    $(CURRENT_DIR)/../src/hynoc_local_interface.v \
                    $(CURRENT_DIR)/../src/dclkfifolut.v \
                    $(CURRENT_DIR)/../src/bin2gray.v \
                    $(CURRENT_DIR)/../src/gray2bin.v

TOP_DEPS          = $(CURRENT_DIR)/../src/hynoc_router_base.v \
                    $(CURRENT_DIR)/../src/hynoc_ingress_routing_ucast.v \
                    $(CURRENT_DIR)/../src/hynoc_ingress_routing_mcast.v \
                    $(CURRENT_DIR)/../src/hynoc_ingress.v \
                    $(CURRENT_DIR)/../src/hynoc_egress.v \
                    $(CURRENT_DIR)/../src/hynoc_prra.v \
                    $(CURRENT_DIR)/../src/prra_lut.v \
                    $(CURRENT_DIR)/../src/sclkfifolut.v \
                    $(CURRENT_DIR)/../src/dclkfifolut.v \
                    $(CURRENT_DIR)/../src/bin2gray.v \
                    $(CURRENT_DIR)/../src/gray2bin.v

include $(CURRENT_DIR)/../../common/Makefiles/iverilog.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriflat.mk
include $(CURRENT_DIR)/../../common/Makefiles/veriobf.mk
include $(CURRENT_DIR)/../../common/Makefiles/common.mk
