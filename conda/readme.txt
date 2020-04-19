Installation
============

Veriparse uses the Miniconda packaging system. Please run the following instructions:

    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
    bash Miniconda3-latest-Linux-x86_64.sh -b -p $HOME/miniconda
    eval "$($HOME/miniconda/bin/conda shell.bash hook)"
    conda create -y -n veriparse -c https://conda.wavecruncher.net/sharpness-1 libgcc-ng libstdcxx-ng gmp
    conda activate veriparse
    conda install ./veriparse-v1.1.6-h6bb024c_0.tar.bz2
    conda deactivate # exit from veriparse env
    conda deactivate # exit from base env

The veriparse tools base directory is located here: $HOME/miniconda/envs/veriparse

As conda rewrite the rpath in the binaries, there is no need to load the veriparse conda environment
to run the tools.


Update An Existing Installation
===============================

To update an existing conda environment, proceed as follow:

    eval "$($HOME/miniconda/bin/conda shell.bash hook)"
    conda activate veriparse
    conda update -y -c https://conda.wavecruncher.net/sharpness-1 gmp
    conda install --force-reinstall ./veriparse-v1.1.6-h6bb024c_0.tar.bz2
    conda deactivate # exit from veriparse env
    conda deactivate # exit from base env


Running Examples
================

Before all things ensure that the environment variable VERIPARSE_LICENSE_FILE points to your license
file.

To run examples icarus verilog should be available in the PATH. That tool is used to simulate
designs but also as a pre-processor before flattening the design.

    yum install iverilog
or
    apt install iverilog

You can test the tool with the following command line:

    mkdir veriparse-examples
    cd veriparse-examples
    export VERIPARSE_INSTALL_DIR=$HOME/miniconda/envs/veriparse
    make -f $VERIPARSE_INSTALL_DIR/share/veriparse/examples/dclkfifolut/project0/Makefile \
        VERIFLAT=$VERIPARSE_INSTALL_DIR/bin/veriflat \
        VERIOBF=$VERIPARSE_INSTALL_DIR/bin/veriobf \
        clean iverilog_check veriflat_check veriobf_check

An all-in-one script that gather the pre-processor, the flattener and the obfuscation is also
provided. It can be tested as follow:

    veriparse -o test.v \
              -t dclkfifolut \
              -p "{FIFO_WIDTH:}" \
              $VERIPARSE_INSTALL_DIR/share/veriparse/examples/dclkfifolut/src/*.v
