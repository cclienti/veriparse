@echo off
setlocal enabledelayedexpansion

rmdir /s /q build 2>nul
mkdir build
cd build

REM Use the MinGW Makefiles generator: GMP's autotools build (via
REM cmake/external_gmp.cmake) requires a POSIX shell + GCC, not MSVC.
REM gcc_win-64/gxx_win-64 (conda-forge) provide the MinGW toolchain.
cmake %CMAKE_ARGS% ^
  -G "MinGW Makefiles" ^
  -DCMAKE_VERBOSE_MAKEFILE=ON ^
  -DCMAKE_PREFIX_PATH=%PREFIX% ^
  -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
  -DCMAKE_INSTALL_LIBDIR=lib ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ^
  %SRC_DIR%
if errorlevel 1 exit 1

cmake --build . --config Release --parallel %CPU_COUNT%
if errorlevel 1 exit 1

set VERIPARSE_SOURCE_ROOT=%SRC_DIR%
ctest --test-dir . -j %CPU_COUNT% -L unittest --output-on-failure
if errorlevel 1 exit 1

cmake --install . --config Release
if errorlevel 1 exit 1
