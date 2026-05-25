@echo off
setlocal enabledelayedexpansion

REM Drive the build through MSYS2's bash so the full conda activation
REM (including gcc_win-64 / m2w64-sysroot LDFLAGS, PATH, etc.) is in
REM effect. This mirrors conda-forge gmp-feedstock's bld.bat.
REM
REM Generate a temporary conda_build.sh that activates the conda env
REM inside bash, then appends our cross-platform build.sh.

echo source %SYS_PREFIX:\=/%/etc/profile.d/conda.sh    > conda_build.sh
echo conda activate "${PREFIX}"                       >> conda_build.sh
echo conda activate --stack "${BUILD_PREFIX}"         >> conda_build.sh
echo CONDA_PREFIX=${CONDA_PREFIX//\\//}               >> conda_build.sh
type "%RECIPE_DIR%\build.sh"                          >> conda_build.sh

set PREFIX=%PREFIX:\=/%
set BUILD_PREFIX=%BUILD_PREFIX:\=/%
set CONDA_PREFIX=%CONDA_PREFIX:\=/%
set SRC_DIR=%SRC_DIR:\=/%
set MSYSTEM=UCRT64
set MSYS2_PATH_TYPE=inherit
set CHERE_INVOKING=1

bash -lc "./conda_build.sh"
if errorlevel 1 exit 1
