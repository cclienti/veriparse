# Generic Veriflat Makefile
# Copyright (C) 2013-2019 Christophe Clienti - All Rights Reserved

VERIFLAT           = veriflat
VERIFLAT_FLAGS    += --seed 0
VERIFLAT_OUTPUT    = $(TOP_MODULE)_flat.v
VERIFLAT_TESTBENCH = $(TESTBENCH_MODULE)_flat

veriflat_check: $(VERIFLAT_TESTBENCH)
	vvp ./$< -lxt2 | tee output.log
	@! grep 'Error\|ERROR\|error' output.log

$(VERIFLAT_TESTBENCH): $(VERIFLAT_OUTPUT)
	$(IVERILOG) $(IVFLAGS) -s $(TESTBENCH_MODULE) -o $@ \
		$(sort $^ $(TESTBENCH_DEPS) $(TESTBENCH_FILE))

$(VERIFLAT_OUTPUT): $(sort $(TOP_DEPS) $(TOP_FILE))
	$(VERIFLAT) $(VERIFLAT_FLAGS) --top-module $(TOP_MODULE) $^ --output $@

clean:: veriflat_clean

veriflat_clean:
	rm -rf $(VERIFLAT_OUTPUT) $(VERIFLAT_TESTBENCH) veriflat.log
