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
CONDA_BUILD_ENV_PATH  ?= $(CURDIR)/pkg-build/env
CONDA_BUILD_ROOT      ?= $(HOME)/veriparse-conda-bld

# Name of the environment for development
CONDA_DEV_ENVIRONMENT ?= veriparse-dev
# Development build type
CONDA_DEV_BUILD_TYPE  ?= RelWithDebInfo
# Development conda environment path (local, no root needed)
CONDA_DEV_ENV_PATH    ?= $(CURDIR)/$(DEV_BUILD_DIR)/env

# Conda Destination Repository Base Path
CONDA_DEST_PATH        ?= $(HOME)/veriparse-packages
# Conda Destination Channel within the Base Path
CONDA_DEST_CHANNEL     ?= veriparse-$(shell date +%Y)


##################################################################
# Local variables
##################################################################

CONDA_DEST_REPO      = $(CONDA_DEST_PATH)/$(CONDA_DEST_CHANNEL)

CONDA_BUILD          = conda-build
CONDA_BUILD_CHANNELS = -c $(CONDA_DEST_REPO) -c conda-forge


CONDA_INDEX          = micromamba index --no-progress -n $(CONDA_DEST_CHANNEL)

NUM_CORES            = $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
# ctest picks this up natively — override with `make dev-test CTEST_PARALLEL_LEVEL=N`
# or `CTEST_PARALLEL_LEVEL=N make dev-test`. `?=` honors both.
CTEST_PARALLEL_LEVEL ?= $(NUM_CORES)
MAMBA                = micromamba
DEV_BUILD_DIR        = build
REPO_ROOT            = $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# CTest label filter for dev-test (unittest, cosim, or regex)
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
	  grep '^ *- ' | grep -v '{{\|compiler\|stdlib\|mold' | \
	  grep -vE '# *\[(win|build_platform.startswith\("win-"\))\]' | \
	  sed 's/^ *- /  - /' >> conda/environment.yml
	@echo "  - verilator"             >> conda/environment.yml

dev-env: dev-env-file
	set -e; \
	  export PATH=$(CONDA_DEV_ENV_PATH)/bin:$$PATH; \
	  $(MAMBA) create -y -p $(CONDA_DEV_ENV_PATH) \
	    --file conda/environment.yml
DEV_RUN = $(MAMBA) run -p $(CONDA_DEV_ENV_PATH)

dev-cmake:
	$(DEV_RUN) cmake -G Ninja \
	      -S $(REPO_ROOT) -B $(DEV_BUILD_DIR) \
	      -DCMAKE_PREFIX_PATH=$(CONDA_DEV_ENV_PATH) \
	      -DCMAKE_BUILD_TYPE=$(CONDA_DEV_BUILD_TYPE) \
	      -DCMAKE_EXE_LINKER_FLAGS="$(DEV_LINKER_FLAGS)" \
	      -DCMAKE_SHARED_LINKER_FLAGS="$(DEV_LINKER_FLAGS)" \
	      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	      -DCMAKE_INSTALL_PREFIX=$(HOME)/veriparse

# Ninja saturates available cores by default — no --parallel needed.
dev-build:
	$(DEV_RUN) cmake --build $(DEV_BUILD_DIR)

dev-test:
	$(DEV_RUN) env CTEST_PARALLEL_LEVEL=$(CTEST_PARALLEL_LEVEL) VERIPARSE_SOURCE_ROOT=$(REPO_ROOT) \
	  ctest --test-dir $(DEV_BUILD_DIR) -L '$(CTEST_LABELS)'

dev-test-cosim:
	$(DEV_RUN) env CTEST_PARALLEL_LEVEL=$(CTEST_PARALLEL_LEVEL) VERIPARSE_SOURCE_ROOT=$(REPO_ROOT) \
	  ctest --test-dir $(DEV_BUILD_DIR) -L 'cosim'

# Paths that astgen.py (re)writes from lib/tools/ASTGen/verilog_ast.yaml.
AST_GEN_PATHS = lib/include/veriparse/AST lib/src/AST \
                lib/include/veriparse/generators lib/src/generators \
                lib/include/veriparse/importers lib/src/importers \
                lib/test/generators lib/test/importers

# Regenerate the C++ AST classes (+ YAML/dot generators, importers, tests)
# from verilog_ast.yaml. --clang-format makes the output match the committed
# (clang-formatted) tree; astgen's format-aware compare leaves unchanged
# files (and their mtimes) untouched.
ast-gen:
	$(DEV_RUN) python lib/tools/ASTGen/astgen.py --clang-format

# CI guard: fail if the committed generated AST is out of sync with
# verilog_ast.yaml or the templates (regenerate, then check for any diff).
ast-check: ast-gen
	@git diff --exit-code -- $(AST_GEN_PATHS) \
	  || { echo "Generated AST is stale vs verilog_ast.yaml — run 'make ast-gen' and commit."; exit 1; }

dev-clean:
	set -e; \
	  rm -rf $(DEV_BUILD_DIR); \
	  $(MAMBA) env remove -y -p $(CONDA_DEV_ENV_PATH)

install-hooks:
	cp scripts/pre-commit .git/hooks/pre-commit
	@echo "pre-commit hook installed"



##################################################################
# Pkg Rules
##################################################################

env:
	$(MAMBA) create -y -p $(CONDA_BUILD_ENV_PATH) -c conda-forge conda-build

package: env
	mkdir -p $(CONDA_DEST_REPO)
	$(MAMBA) run -p $(CONDA_BUILD_ENV_PATH) \
	  $(CONDA_BUILD) --croot $(CONDA_BUILD_ROOT) $(CONDA_BUILD_CHANNELS) --output-folder $(CONDA_DEST_REPO) conda/recipe-$(BUILD_TYPE)

package-name:
	@$(MAMBA) run -p $(CONDA_BUILD_ENV_PATH) \
	  $(CONDA_BUILD) --croot $(CONDA_BUILD_ROOT) --output-folder $(CONDA_DEST_REPO) --output conda/recipe-$(BUILD_TYPE) 2>/dev/null

index:
	$(MAMBA) run -p $(CONDA_BUILD_ENV_PATH) \
	  $(CONDA_INDEX) $(CONDA_DEST_REPO)

clean:
	$(MAMBA) run -p $(CONDA_BUILD_ENV_PATH) conda-build --croot $(CONDA_BUILD_ROOT) purge || true
	$(MAMBA) env remove -y -p $(CONDA_BUILD_ENV_PATH)
	rm -rf pkg-build


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
	@echo "  dev-test-cosim        Run verilator-based cosim tests"
	@echo "  ast-gen               Regenerate the C++ AST from verilog_ast.yaml (clang-format)"
	@echo "  ast-check             Fail if the committed generated AST is stale vs the yaml"
	@echo "  dev-clean             Remove build directory and dev conda environment"
	@echo "  install-hooks         Install git pre-commit hook (clang-format check)"
	@echo ""
	@echo "Pkg Rules:"
	@echo "  env          Create the conda build environment"
	@echo "  package      Build the conda package"
	@echo "  package-name Print the conda package name"
	@echo "  index        Index the conda destination repository"
	@echo "  clean        Remove the build environment and purge conda build cache"
	@echo ""
	@echo "Common Rules:"
	@echo "  distclean    Run clean and dev-clean"
	@echo "  print-%%     Print the value of a Makefile variable"
	@echo "  help         Show this help message"
