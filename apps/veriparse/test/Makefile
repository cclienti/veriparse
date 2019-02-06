TESTS	= tiny_aes_128 tiny_aes_192 tiny_aes_256


all:
	@for test in $(TESTS); do \
		echo -e "========================="; \
		echo -e "= Test $(TESTS)"; \
		echo -e "========================="; \
		make VERIFLAT=$(VERIFLAT) -C $$test/build iverilog_check veriflat_check veriobf_check; \
		echo -e ""; \
	done

clean:
	for test in $(TESTS); do make -C $$test/build clean; done
