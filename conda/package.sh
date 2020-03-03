#!/bin/bash

# Set default conda path
CONDA_DISTRIB=/opt/miniconda3
if [ "$#" -ge 1 ]; then
    CONDA_DISTRIB=$1
fi
echo "CONDA_DISTRIB=$CONDA_DISTRIB"

# Set default conda repository
CONDA_REPOSITORY=https://conda.wavecruncher.net/sharpness-1
if [ "$#" -ge 2 ]; then
    CONDA_REPOSITORY=$2
fi
echo "CONDA_REPOSITORY=$CONDA_REPOSITORY"

# Set default conda repository
CONDA_TARGET=release
if [ "$#" -ge 3 ]; then
    CONDA_TARGET=$3
fi
echo "CONDA_TARGET=$CONDA_TARGET"

# Load conda main environment
eval "$($CONDA_DISTRIB/bin/conda shell.bash hook)"

# Clean previous build
conda clean --all -y
conda build purge

# Clean previous build
conda-build -c $CONDA_REPOSITORY $(dirname $0)/recipe-$CONDA_TARGET
