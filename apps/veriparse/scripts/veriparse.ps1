# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2013-2026 Christophe Clienti
#
# Windows port of scripts/veriparse (bash). Same flow: preprocess (iverilog),
# flatten (veriflat), obfuscate (veriobf). iverilog.exe must be on PATH.

[CmdletBinding()]
param(
    [Alias('o')]
    [string]$Output,

    [Alias('t')]
    [string]$TopModule,

    [Alias('I')]
    [string[]]$IncludeDir = @(),

    [Alias('p')]
    [string]$ParamMap = "{}",

    [Alias('e')]
    [switch]$DeadcodeEnd,

    [Alias('d')]
    [switch]$DeadcodeDuring,

    [Alias('s')]
    [int]$Seed = 0,

    [Alias('l')]
    [int]$IdLength = 16,

    [Alias('a')]
    [switch]$Hash,

    [Alias('n')]
    [switch]$NoObfuscate,

    [Alias('h')]
    [switch]$Help,

    [Alias('v')]
    [switch]$Version,

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$InputFiles
)

function Show-Usage {
    Write-Host ""
    Write-Host "usage: veriparse.ps1 [OPTIONS]... VERILOG_FILES..."
    Write-Host ""
    Write-Host "Preprocess, flatten and obfuscate input files into one output file."
    Write-Host ""
    Write-Host "Required options:"
    Write-Host "  -o, -Output FILE         Output file."
    Write-Host "  -t, -TopModule TOP       Top module to consider."
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -h, -Help                Display this help."
    Write-Host "  -v, -Version             Display version number."
    Write-Host "  -I, -IncludeDir DIR      Include directory (repeatable)."
    Write-Host "  -p, -ParamMap YAML       YAML map for verilog parameters,"
    Write-Host "                           example: '{MODULE_PARAM1: 1, MODULE_PARAM2: 2}'."
    Write-Host "  -e, -DeadcodeEnd         Apply deadcode elimination after flattening."
    Write-Host "  -d, -DeadcodeDuring      Apply deadcode elimination during flattening."
    Write-Host "  -s, -Seed INT            RNG seed (default: 0)."
    Write-Host "  -l, -IdLength INT        Max length of obfuscated identifiers (default: 16)."
    Write-Host "  -a, -Hash                Use hashed identifiers instead of random ones."
    Write-Host "  -n, -NoObfuscate         Do not obfuscate the output."
    Write-Host ""
}

if ($Help) { Show-Usage; exit 0 }

if ($Version) {
    & veriflat --version --output NUL --top null
    exit $LASTEXITCODE
}

if (-not $InputFiles -or $InputFiles.Count -eq 0) {
    Write-Host "No input file(s) given"
    Show-Usage
    exit 1
}
if (-not $TopModule) {
    Write-Host "No top module given"
    Show-Usage
    exit 1
}
if (-not $Output) {
    Write-Host "No output file given"
    Show-Usage
    exit 1
}

$includeArgs = @()
foreach ($d in $IncludeDir) { $includeArgs += "-I$d" }

$ivflags = @('-Wall', '-Wno-sensitivity-entire-array', '-g2005')

Write-Host "Options:"
Write-Host " - Input files: $($InputFiles -join ' ')"
Write-Host " - Output: $Output"
Write-Host " - Top module: $TopModule"
Write-Host " - Top Module Parameters map: ""$ParamMap"""
Write-Host " - Random generator seed: $Seed"
Write-Host " - Do not obfuscate: $($NoObfuscate.IsPresent)"
Write-Host " - Iverilog includes: $($includeArgs -join ' ')"
Write-Host " - Obfuscated identifier length: $IdLength"
if ($DeadcodeEnd)    { Write-Host " - Deadcode elimination at end" }
if ($DeadcodeDuring) { Write-Host " - Deadcode elimination during" }
if ($Hash)           { Write-Host " - Obfuscation uses hash" }
Write-Host ""

# Preprocess
Write-Host "-= Preprocessing verilog files =-"
& iverilog @ivflags @includeArgs -E @InputFiles -o $Output
if ($LASTEXITCODE -ne 0) {
    Write-Host "Preprocessing error"
    exit $LASTEXITCODE
}

# Flatten
Write-Host "-= Flatenning $TopModule to $Output =-"
$flatArgs = @('--top-module', $TopModule, '--output', $Output, '--param-map', $ParamMap, '--seed', $Seed)
if ($DeadcodeEnd)    { $flatArgs += '--deadcode-end' }
if ($DeadcodeDuring) { $flatArgs += '--deadcode-during' }
$flatArgs += $Output

& veriflat @flatArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Flattening error"
    exit $LASTEXITCODE
}

if ($NoObfuscate) { exit 0 }

# Obfuscate
Write-Host "-= Obfuscating $Output =-"
$obfArgs = @('--output', $Output, '--seed', $Seed, '--id-length', $IdLength)
if ($Hash) { $obfArgs += '--hash' }
$obfArgs += $Output

& veriobf @obfArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "Obfuscation error"
    exit $LASTEXITCODE
}
