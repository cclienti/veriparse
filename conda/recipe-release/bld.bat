@echo off
setlocal enabledelayedexpansion

mkdir build
cd build

cmake %CMAKE_ARGS% ^
    -G "Visual Studio 17 2022" -A x64 -T ClangCL ^
    -DCMAKE_PREFIX_PATH="%PREFIX%/Library" ^
    -DCMAKE_INSTALL_PREFIX="%PREFIX%/Library" ^
    -DCMAKE_BUILD_TYPE=Release ^
    "%SRC_DIR%"
if errorlevel 1 exit /b 1

cmake --build . --config Release --parallel
if errorlevel 1 exit /b 1

set VERIPARSE_SOURCE_ROOT=%SRC_DIR%
ctest -j 4 -L unittest --output-on-failure -C Release
if errorlevel 1 exit /b 1

cmake --install . --config Release
if errorlevel 1 exit /b 1
