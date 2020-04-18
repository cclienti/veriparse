##################################################################
# Options
##################################################################

# Sharpness Conda Repository
SHARPNESS_REPOSITORY   ?= https://conda.wavecruncher.net/sharpness-1

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


##################################################################
# Dev Rules
##################################################################

dev-env:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  deps=`grep -v "{{" recipe-release/meta.yaml | \
	        python -c 'import yaml; import sys; \
                d = yaml.load(sys.stdin, Loader=yaml.FullLoader);\
                print(" ".join(d["requirements"]["build"]))'`; \
	  conda create -y -n $(CONDA_DEV_ENVIRONMENT) -c $(SHARPNESS_REPOSITORY) $$deps

dev-cmake:
	mkdir -p build
	set -e; \
	  cd build; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_DEV_ENVIRONMENT); \
	  cmake -DVERIPARSE_EXTERNAL_ROOT=${CONDA_PREFIX} \
	        -DCMAKE_BUILD_TYPE=$(CONDA_DEV_BUILD_TYPE) ../..

dev-build:
	set -e; \
	  cd build; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_DEV_ENVIRONMENT); \
	  make -j${NUM_CORES}

dev-test:
	set -e; \
	  cd build; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_DEV_ENVIRONMENT); \
	  VERIPARSE_SOURCE_ROOT=$(realpath ..) ctest -j${NUM_CORES}

dev-clean:
	set -e; \
	  rm -rf build; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda env remove -y -n $(CONDA_DEV_ENVIRONMENT)


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
	  $(CONDA_BUILD) $(CONDA_BUILD_CHANNELS) --output-folder $(CONDA_DEST_REPO) recipe-$(BUILD_TYPE)

package-name:
	@set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_BUILD_ENVIRONMENT); \
	  $(CONDA_BUILD) --output-folder $(CONDA_DEST_REPO) --output recipe-$(BUILD_TYPE) 2>/dev/null

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
