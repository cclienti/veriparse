# Windows dev workflow for veriparse.
#
# Usage:
#   .\dev.ps1 <target> [-BuildType <cfg>] [-Labels <regex>]
#
# Targets:
#   env-file        Generate conda\environment.yml from meta.yaml (Windows
#                   selectors).
#   env             Create the dev conda env under .\build\env via micromamba.
#                   Runs env-file first.
#   cmake           Configure the project with VS17 2022 + ClangCL.
#   build           Build the active configuration.
#   test            Run ctest with -L <Labels> (default: unittest).
#   test-cosim      ctest with -L cosim (no-op on Windows: verilator is
#                   only packaged for Linux/macOS on conda-forge).
#   clean           Remove .\build (env + cmake state).
#   all             env -> cmake -> build -> test.
#
# Defaults match the Makefile: build dir is .\build, env at .\build\env,
# build type RelWithDebInfo. Override with -BuildType / -Labels.

[CmdletBinding()]
param(
    [Parameter(Position = 0, Mandatory = $true)]
    [ValidateSet('env-file', 'env', 'cmake', 'build', 'test', 'test-cosim', 'clean', 'all')]
    [string]$Target,

    [string]$BuildType = 'RelWithDebInfo',
    [string]$Labels = 'unittest'
)

$ErrorActionPreference = 'Stop'

$RepoRoot   = $PSScriptRoot
$BuildDir   = Join-Path $RepoRoot 'build'
$EnvPath    = Join-Path $BuildDir 'env'
$EnvPrefix  = Join-Path $EnvPath 'Library'   # conda Windows layout
$MetaYaml   = Join-Path $RepoRoot 'conda\recipe-release\meta.yaml'
$EnvYaml    = Join-Path $RepoRoot 'conda\environment.yml'

function Invoke-Native {
    param([string]$Cmd, [string[]]$ArgList)
    & $Cmd @ArgList
    if ($LASTEXITCODE -ne 0) {
        throw "$Cmd exited with code $LASTEXITCODE"
    }
}

function New-EnvironmentFile {
    # Extract the `requirements.build` block from meta.yaml and keep deps
    # whose # [selector] matches Windows (or has no selector). Skips Jinja
    # template lines ({{ compiler... }}) and the Unix-only mold/flex/bison.
    $skipSelectors = @('not win', 'linux', 'osx', 'unix')

    $lines    = Get-Content -Path $MetaYaml -Encoding utf8 -ErrorAction Stop
    $inReqs   = $false
    $inBuild  = $false
    $deps     = New-Object System.Collections.Generic.List[string]

    foreach ($line in $lines) {
        if ($line -match '^requirements:')      { $inReqs = $true; continue }
        if ($inReqs -and $line -match '^\S')    { $inReqs = $false }
        if (-not $inReqs)                       { continue }

        if ($line -match '^  build:')           { $inBuild = $true; continue }
        if ($inBuild -and $line -match '^  \S') { $inBuild = $false }
        if (-not $inBuild)                      { continue }

        if ($line -notmatch '^\s+-\s')          { continue }
        if ($line -match '\{\{|compiler|stdlib') { continue }

        $skip = $false
        if ($line -match '#\s*\[(.+?)\]') {
            $selector = $Matches[1].Trim()
            if ($skipSelectors -contains $selector) { $skip = $true }
        }
        if ($skip) { continue }

        # Normalize leading whitespace to "  - "
        $deps.Add(($line -replace '^\s+-\s+', '  - '))
    }

    $body = @('channels:', '  - conda-forge', 'dependencies:') + $deps
    # Write UTF-8 without BOM via .NET so Windows PowerShell 5.1 (which
    # otherwise emits a BOM under -Encoding utf8) does not surprise conda.
    [System.IO.File]::WriteAllLines($EnvYaml, $body, [System.Text.UTF8Encoding]::new($false))
    Write-Host "Generated $EnvYaml" -ForegroundColor Cyan
}

function New-CondaEnv {
    New-EnvironmentFile
    if (-not (Get-Command micromamba -ErrorAction SilentlyContinue)) {
        throw "micromamba not found on PATH. Install it first (e.g. winget install Anaconda.Micromamba)."
    }
    Invoke-Native 'micromamba' @('create', '-y', '-p', $EnvPath, '--file', $EnvYaml)
}

function Enable-CondaEnvForCurrentProcess {
    # Mimic `micromamba activate` without touching shell profile state.
    # Prepend the env's bin dirs and set CONDA_PREFIX so cmake's find_package
    # picks up Boost/yaml-cpp/gtest from the conda env, and Windows loader
    # finds runtime DLLs at test discovery time.
    if (-not (Test-Path $EnvPath)) {
        throw "Conda env not found at $EnvPath. Run '.\dev.ps1 env' first."
    }
    $env:CONDA_PREFIX = $EnvPrefix
    $env:PATH = "$EnvPrefix\bin;$EnvPath\Scripts;$EnvPath;$env:PATH"
}

function Invoke-CmakeConfigure {
    Enable-CondaEnvForCurrentProcess
    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    Push-Location $BuildDir
    try {
        Invoke-Native 'cmake' @(
            '-G', 'Visual Studio 17 2022', '-A', 'x64', '-T', 'ClangCL',
            "-DCMAKE_PREFIX_PATH=$EnvPrefix",
            "-DCMAKE_BUILD_TYPE=$BuildType",
            '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
            "-DCMAKE_INSTALL_PREFIX=$env:USERPROFILE\veriparse",
            $RepoRoot
        )
    } finally { Pop-Location }
}

function Invoke-CmakeBuild {
    Enable-CondaEnvForCurrentProcess
    Push-Location $BuildDir
    try {
        Invoke-Native 'cmake' @('--build', '.', '--config', $BuildType, '--parallel')
    } finally { Pop-Location }
}

function Invoke-Ctest {
    Enable-CondaEnvForCurrentProcess
    Push-Location $BuildDir
    try {
        $env:VERIPARSE_SOURCE_ROOT = $RepoRoot
        Invoke-Native 'ctest' @('-j', '4', '-C', $BuildType, '-L', $Labels, '--output-on-failure')
    } finally { Pop-Location }
}

function Remove-DevBuild {
    if (Test-Path $BuildDir) {
        Write-Host "Removing $BuildDir" -ForegroundColor Yellow
        Remove-Item -Recurse -Force $BuildDir
    }
}

switch ($Target) {
    'env-file'         { New-EnvironmentFile }
    'env'              { New-CondaEnv }
    'cmake'            { Invoke-CmakeConfigure }
    'build'            { Invoke-CmakeBuild }
    'test'             { Invoke-Ctest }
    'test-cosim'       { $Labels = 'cosim'; Invoke-Ctest }
    'clean'            { Remove-DevBuild }
    'all'              {
        New-CondaEnv
        Invoke-CmakeConfigure
        Invoke-CmakeBuild
        Invoke-Ctest
    }
}
