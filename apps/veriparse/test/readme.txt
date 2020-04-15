# Before all things ensure that the environment variable VERIPARSE_LICENSE_FILE points to your
# license file.

# To run examples icarus verilog should be available in the PATH. That tool is used to simulate
# designs but also as a pre-processor before flattening the design.  You can test the tool with the
# following command line:

VERIPARSE_INSTALL_DIR=/opt/veriparse
make -f $VERIPARSE_PREFIX_PATH/share/veriparse/examples/dclkfifolut/project0/Makefile \
        VERIFLAT=$VERIPARSE_PREFIX_PATH/bin/veriflat \
        VERIOBF=$VERIPARSE_PREFIX_PATH/bin/veriobf \
        clean iverilog_check veriflat_check veriobf_check
