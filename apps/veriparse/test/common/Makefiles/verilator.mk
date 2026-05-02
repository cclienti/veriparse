# Generic Verilator Lint Makefile
# Copyright (C) 2013-2019 Christophe Clienti - All Rights Reserved

VERILATOR        = verilator
VERILATOR_FLAGS += --lint-only -Wno-fatal $(foreach dir,$(INCLUDE_DIRS),+incdir+$(dir))

verilator_check: $(VERIFLAT_OUTPUT)
	@echo "---- Verilator lint check ----"
	$(VERILATOR) $(VERILATOR_FLAGS) --top-module $(TOP_MODULE) $<

clean:: verilator_clean

verilator_clean:
	rm -rf verilator.log
