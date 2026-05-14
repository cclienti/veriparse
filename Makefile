SHELL := /bin/bash
##################################################################
# Options
##################################################################

# Build type (release or debug)
BUILD_TYPE             ?= release

# Conda Distribution Path
CONDA_DISTRIB_PATH      ?= $(shell conda info --base)
# Name of the environment to build packages
CONDA_BUILD_ENVIRONMENT ?= veriparse-$(BUILD_TYPE)

# Name of the environment for development
CONDA_DEV_ENVIRONMENT ?= veriparse-dev
# Development build type
CONDA_DEV_BUILD_TYPE  ?= RelWithDebInfo
# Development conda environment path (local, no root needed)
CONDA_DEV_ENV_PATH    ?= $(CURDIR)/$(DEV_BUILD_DIR)/env

# Conda Destination Repository Base Path
CONDA_DEST_PATH        ?= /opt/veriparse-packages
# Conda Destination Channel within the Base Path
CONDA_DEST_CHANNEL     ?= veriparse-$(shell date +%Y)


##################################################################
# Local variables
##################################################################

CONDA_DEST_REPO      = $(CONDA_DEST_PATH)/$(CONDA_DEST_CHANNEL)

CONDA_BUILD          = conda-build
CONDA_BUILD_CHANNELS = -c $(CONDA_DEST_REPO) -c $(SHARPNESS_REPOSITORY)

CONDA_VERIFY         = conda-verify

CONDA_INDEX          = conda-index --no-progress -n $(CONDA_DEST_CHANNEL)

NUM_CORES            = $(shell nproc)
MAMBA                = micromamba
DEV_BUILD_DIR        = build
REPO_ROOT            = $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# CTest label filter for dev-test (unittest, verilator, integration, or regex)
CTEST_LABELS ?= unittest
DEV_LINKER_FLAGS     ?= -fuse-ld=mold



##################################################################
# Dev Rules
##################################################################

dev-env-file: conda/recipe-release/meta.yaml
	@echo "channels:"                   > conda/environment.yml
	@echo "  - conda-forge"            >> conda/environment.yml
	@echo "dependencies:"              >> conda/environment.yml
	@sed -n '/^requirements:/,/^[^ ]/p' conda/recipe-release/meta.yaml | \
	  sed -n '/^  build:/,/^  [^ ]/p' | \
	  grep '^ *- ' | grep -v '{{\|compiler\|mold' | \
	  sed 's/^ *- /  - /' >> conda/environment.yml
	@echo "  - iverilog"              >> conda/environment.yml
	@echo "  - verilator"             >> conda/environment.yml

dev-env: dev-env-file
	set -e; \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  $(MAMBA) create -y -p $(CONDA_DEV_ENV_PATH) \
	    --file conda/environment.yml
dev-cmake:
	mkdir -p $(DEV_BUILD_DIR)
	set -e; \
	  cd $(DEV_BUILD_DIR); \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  export CONDA_PREFIX=$(CONDA_DEV_ENV_PATH); \
	  cmake -DVERIPARSE_EXTERNAL_ROOT=$$CONDA_PREFIX \
	        -DCMAKE_BUILD_TYPE=$(CONDA_DEV_BUILD_TYPE) \
	        -DCMAKE_EXE_LINKER_FLAGS="$(DEV_LINKER_FLAGS)" \
	        -DCMAKE_SHARED_LINKER_FLAGS="$(DEV_LINKER_FLAGS)" \
	        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	        -DCMAKE_INSTALL_PREFIX=$$HOME/veriparse $(REPO_ROOT)

dev-build:
	set -e; \
	  cd $(DEV_BUILD_DIR); \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  export CONDA_PREFIX=$(CONDA_DEV_ENV_PATH); \
	  make -j${NUM_CORES}

dev-test:
	set -e; \
	  cd $(DEV_BUILD_DIR); \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  export CONDA_PREFIX=$(CONDA_DEV_ENV_PATH); \
	  VERIPARSE_SOURCE_ROOT=$(REPO_ROOT) ctest -j${NUM_CORES} -L '$(CTEST_LABELS)'

dev-test-integration:
	set -e; \
	  cd $(DEV_BUILD_DIR); \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  export CONDA_PREFIX=$(CONDA_DEV_ENV_PATH); \
	  VERIPARSE_SOURCE_ROOT=$(REPO_ROOT) ctest -j${NUM_CORES} -L 'verilator|integration'

dev-clean:
	set -e; \
	  rm -rf $(DEV_BUILD_DIR); \
	  $(MAMBA) env remove -y -p $(CONDA_DEV_ENV_PATH)


##################################################################
# Pkg Rules
##################################################################

env:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda create -y -n $(CONDA_BUILD_ENVIRONMENT) conda-build conda-verify

package:
	mkdir -p $(CONDA_DEST_REPO)
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_BUILD_ENVIRONMENT); \
	  $(CONDA_BUILD) $(CONDA_BUILD_CHANNELS) --output-folder $(CONDA_DEST_REPO) conda/recipe-$(BUILD_TYPE)

package-name:
	@set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_BUILD_ENVIRONMENT); \
	  $(CONDA_BUILD) --output-folder $(CONDA_DEST_REPO) --output conda/recipe-$(BUILD_TYPE) 2>/dev/null

verify:
	set -e; \
	  CONDA_PACKAGE_NAME=`$(MAKE) -s package-name`; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_BUILD_ENVIRONMENT); $(CONDA_VERIFY) $$CONDA_PACKAGE_NAME

index:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_BUILD_ENVIRONMENT); $(CONDA_INDEX) $(CONDA_DEST_REPO)

clean:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda env remove -y -n $(CONDA_BUILD_ENVIRONMENT); \
	  conda build purge


##################################################################
# Common Rules
##################################################################

distclean: clean dev-clean

print-%:
	@echo $*=$($*)

help:
	@echo "Dev Rules:"
	@echo "  dev-env-file Generate conda/environment.yml from recipe deps"
	@echo "  dev-env      Create the development conda environment"
	@echo "  dev-cmake    Run CMake configuration in the dev environment"
	@echo "  dev-build    Build the project in the dev environment"
	@echo "  dev-test              Run unittest tests (override with CTEST_LABELS=...)"
	@echo "  dev-test-integration  Run verilator and integration tests"
	@echo "  dev-clean             Remove build directory and dev conda environment"
	@echo ""
	@echo "Pkg Rules:"
	@echo "  env          Create the conda build environment"
	@echo "  package      Build the conda package"
	@echo "  package-name Print the conda package name"
	@echo "  verify       Verify the conda package"
	@echo "  index        Index the conda destination repository"
	@echo "  clean        Remove the build environment and purge conda build cache"
	@echo ""
	@echo "Common Rules:"
	@echo "  distclean    Run clean and dev-clean"
	@echo "  print-%%     Print the value of a Makefile variable"
	@echo "  help         Show this help message"
