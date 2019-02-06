# Generic Veriobf Makefile
# Copyright (C) 2013-2019 Christophe Clienti - All Rights Reserved

VERIOBF           = veriobf
VERIOBF_FLAGS    += --id-length 16 --seed 0
VERIOBF_INPUT     = $(TOP_MODULE)_flat.v
VERIOBF_OUTPUT    = $(TOP_MODULE)_obf.v
VERIOBF_TESTBENCH = $(TESTBENCH_MODULE)_obf

veriobf_check: $(VERIOBF_TESTBENCH)
	@echo -e "---- Test veriobf ----"
	@vvp ./$< -lxt2 | tee output.log
	@! grep 'Error\|ERROR\|error' output.log

$(VERIOBF_TESTBENCH): $(VERIOBF_OUTPUT)
	$(IVERILOG) $(IVFLAGS) -s $(TESTBENCH_MODULE) -o $@ \
		$(sort $^ $(TESTBENCH_DEPS) $(TESTBENCH_FILE))

$(VERIOBF_OUTPUT): $(VERIOBF_INPUT)
	$(VERIOBF) $(VERIOBF_FLAGS) $^ --output $@

clean:: veriobf_clean

veriobf_clean:
	rm -rf $(VERIOBF_OUTPUT) $(VERIOBF_TESTBENCH) veriobf.log
