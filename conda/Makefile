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
CONDA_DEV_BUILD_TYPE  ?= Release # Debug

# Conda Destination Repository Base Path
CONDA_DEST_PATH        ?= ~/veriparse-packages
# Conda Destination Channel within the Base Path
CONDA_DEST_CHANNEL     ?= veriparse-$(shell date +%Y)


##################################################################
# Local variables
##################################################################

CONDA_DEST_REPO      = $(CONDA_DEST_PATH)/$(CONDA_DEST_CHANNEL)

CONDA_BUILD          = conda-build
CONDA_BUILD_CHANNELS = -c conda-forge # $(CONDA_DEST_REPO) -c

CONDA_VERIFY         = conda-verify

CONDA_INDEX          = conda-index --no-progress -n $(CONDA_DEST_CHANNEL)

NUM_CORES            = $(shell nproc)


##################################################################
# Dev Rules
##################################################################

dev-env:
	set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  deps=`grep -v "{{" recipe-release/meta.yaml | \
	        $(CONDA_DISTRIB_PATH)/bin/python -c 'import yaml; import sys; \
                d = yaml.load(sys.stdin, Loader=yaml.FullLoader);\
                print(" ".join(d["requirements"]["build"]))'`; \
	  mamba create -y -n $(CONDA_DEV_ENVIRONMENT) $$deps

dev-cmake:
	mkdir -p build
	set -e; \
	  cd build; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_DEV_ENVIRONMENT); \
	  cmake -DVERIPARSE_EXTERNAL_ROOT=${CONDA_PREFIX} \
	        -DCMAKE_BUILD_TYPE=$(CONDA_DEV_BUILD_TYPE) ../..

dev-build:
	set -e; \
	  cd build; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_DEV_ENVIRONMENT); \
	  make -j${NUM_CORES}

dev-test:
	set -e; \
	  cd build; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_DEV_ENVIRONMENT); \
	  VERIPARSE_SOURCE_ROOT=$(realpath ..) ctest -j${NUM_CORES}

dev-clean:
	set -e; \
	  rm -rf build; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba env remove -y -n $(CONDA_DEV_ENVIRONMENT)


##################################################################
# Pkg Rules
##################################################################

env:
	set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba create -y -n $(CONDA_BUILD_ENVIRONMENT) conda-build conda-verify

package:
	mkdir -p $(CONDA_DEST_REPO)
	set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_BUILD_ENVIRONMENT); \
	  $(CONDA_BUILD) $(CONDA_BUILD_CHANNELS) --output-folder $(CONDA_DEST_REPO) recipe-$(BUILD_TYPE)

package-name:
	@set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_BUILD_ENVIRONMENT); \
	  $(CONDA_BUILD) --output-folder $(CONDA_DEST_REPO) --output recipe-$(BUILD_TYPE) 2>/dev/null

verify:
	set -e; \
	  CONDA_PACKAGE_NAME=`$(MAKE) -s package-name`; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_BUILD_ENVIRONMENT); $(CONDA_VERIFY) $$CONDA_PACKAGE_NAME

index:
	set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba activate $(CONDA_BUILD_ENVIRONMENT); $(CONDA_INDEX) $(CONDA_DEST_REPO)

clean:
	set -e; \
	  eval "$$(mamba shell hook --shell bash)"; \
	  mamba env remove -y -n $(CONDA_BUILD_ENVIRONMENT); \
	  conda build purge


##################################################################
# Common Rules
##################################################################

distclean: clean dev-clean

print-%:
	@echo $*=$($*)
