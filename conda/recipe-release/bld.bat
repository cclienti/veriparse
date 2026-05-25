@echo off
setlocal enabledelayedexpansion

REM Veriparse Windows build pipeline.
REM
REM Toolchain: MSVC (the default conda-build compiler on Windows).
REM conda-forge provides MSVC-built libboost, yaml-cpp, gtest, etc.,
REM so the whole stack stays on a single C++ ABI.
REM
REM GMP comes from vcpkg's gmp port (declared in vcpkg.json). The
REM conda-forge gmp package on win-64 does not ship gmpxx, so we
REM install GMP+gmpxx via vcpkg and let CMake pick it up via the
REM vcpkg toolchain.

set "VCPKG_ROOT=%SRC_DIR%\vcpkg"

if not exist "%VCPKG_ROOT%" (
  git clone --depth 1 https://github.com/microsoft/vcpkg.git "%VCPKG_ROOT%"
  if errorlevel 1 exit /b 1
  call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics
  if errorlevel 1 exit /b 1
)

rmdir /s /q build 2>nul
mkdir build
cd build

cmake %CMAKE_ARGS% ^
  -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows ^
  -DCMAKE_PREFIX_PATH=%PREFIX% ^
  -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
  -DCMAKE_INSTALL_LIBDIR=lib ^
  -DCMAKE_BUILD_TYPE=Release ^
  %SRC_DIR%
if errorlevel 1 exit /b 1

cmake --build . --config Release --parallel %CPU_COUNT%
if errorlevel 1 exit /b 1

set VERIPARSE_SOURCE_ROOT=%SRC_DIR%
ctest --test-dir . -j %CPU_COUNT% -L unittest --output-on-failure -C Release
if errorlevel 1 exit /b 1

cmake --install . --config Release
if errorlevel 1 exit /b 1
