SHARPNESS_CHANNEL  ?= https://conda.wavecruncher.net/sharpness-1
BRANCH             ?= release
CONDA_CHANNEL      ?= veriparse-$(shell date +%Y)
CONDA_BASE_REPO    ?= /opt/veriparse-packages

CONDA_ENV          ?= veriparse-$(BRANCH)

CONDA_REPO          = $(CONDA_BASE_REPO)/$(CONDA_CHANNEL)
CONDA_BUILD         = conda-build
CONDA_BUILD_FLAGS   = -c $(CONDA_REPO) -c $(SHARPNESS_CHANNEL)
CONDA_VERIFY        = conda-verify
CONDA_INDEX         = conda-index --verbose --no-progress -n $(CONDA_CHANNEL)
SHELL               = /bin/bash

env:
	conda create -y -n $(CONDA_ENV) conda-build conda-verify

package:
	mkdir -p $(CONDA_REPO)
	conda run -n $(CONDA_ENV) \
		$(CONDA_BUILD) $(CONDA_BUILD_FLAGS) --output-folder $(CONDA_REPO) recipe-$(BRANCH)

package-name:
	@$(CONDA_BUILD) --output-folder $(CONDA_REPO) --output recipe-$(BRANCH) 2>/dev/null

verify:
	set -e; \
	  CONDA_PACKAGE_NAME=`$(MAKE) -s package-name`; \
	  conda run -n $(CONDA_ENV) $(CONDA_VERIFY) $$CONDA_PACKAGE_NAME

index:
	mkdir -p $(CONDA_REPO)
	$(CONDA_INDEX) $(CONDA_REPO)

clean:
	conda env remove -n $(CONDA_ENV) -y
