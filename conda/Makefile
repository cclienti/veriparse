SHARPNESS_CHANNEL      ?= https://conda.wavecruncher.net/sharpness-1
BRANCH                 ?= release

CONDA_DISTRIB_PATH     ?= $(shell conda info --base)
CONDA_ENVIRONMENT      ?= veriparse-$(BRANCH)

CONDA_DEST_PATH        ?= /opt/veriparse-packages
CONDA_DEST_CHANNEL     ?= veriparse-$(shell date +%Y)

CONDA_DEST_REPO         = $(CONDA_DEST_PATH)/$(CONDA_DEST_CHANNEL)
CONDA_BUILD             = conda-build
CONDA_BUILD_FLAGS       = -c $(CONDA_DEST_REPO) -c $(SHARPNESS_CHANNEL)
CONDA_VERIFY            = conda-verify
CONDA_INDEX             = conda-index --no-progress -n $(CONDA_DEST_CHANNEL)


env:
	mkdir -p $(CONDA_DEST_REPO)
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda create -y -n $(CONDA_ENVIRONMENT) conda-build conda-verify

package:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_ENVIRONMENT); \
	  $(CONDA_BUILD) $(CONDA_BUILD_FLAGS) --output-folder $(CONDA_DEST_REPO) recipe-$(BRANCH)

package-name:
	@set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_ENVIRONMENT); \
	  $(CONDA_BUILD) --output-folder $(CONDA_DEST_REPO) --output recipe-$(BRANCH) 2>/dev/null

verify:
	set -e; \
	  CONDA_PACKAGE_NAME=`$(MAKE) -s package-name`; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_ENVIRONMENT); $(CONDA_VERIFY) $$CONDA_PACKAGE_NAME

index:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda activate $(CONDA_ENVIRONMENT); $(CONDA_INDEX) $(CONDA_DEST_REPO)

clean:
	set -e; \
	  . $(CONDA_DISTRIB_PATH)/etc/profile.d/conda.sh; \
	  conda env remove -y -n $(CONDA_ENVIRONMENT)
