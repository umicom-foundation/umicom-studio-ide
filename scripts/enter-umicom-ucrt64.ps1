# -----------------------------------------------------------------------------
# Umicom Studio IDE
# File: scripts/enter-umicom-ucrt64.ps1
#
# PURPOSE:
#   Prepare the current PowerShell process for native Windows development with
#   the MSYS2 UCRT64 compiler, Ninja, pkg-config, GTK4 and resource tools.  The
#   script must be dot-sourced so its environment changes remain active.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
# -----------------------------------------------------------------------------
[CmdletBinding()]
param(
    [string]$Ucrt64Root = "C:\msys64\ucrt64"
)

$ErrorActionPreference = "Stop"

$BinDirectory = Join-Path $Ucrt64Root "bin"
$RequiredTools = @(
    "clang.exe",
    "ninja.exe",
    "pkg-config.exe",
    "gcc.exe",
    "windres.exe"
)

if (-not (Test-Path -LiteralPath $BinDirectory -PathType Container)) {
    throw "MSYS2 UCRT64 was not found at: $BinDirectory"
}

foreach ($Tool in $RequiredTools) {
    $ToolPath = Join-Path $BinDirectory $Tool
    if (-not (Test-Path -LiteralPath $ToolPath -PathType Leaf)) {
        throw "Required UCRT64 tool is missing: $ToolPath"
    }
}

$PathEntries = $env:Path -split ";" | Where-Object { $_ -ne "" }
$AlreadyFirst = $PathEntries.Count -gt 0 -and
    $PathEntries[0].TrimEnd("\") -ieq $BinDirectory.TrimEnd("\")

if (-not $AlreadyFirst) {
    $env:Path = "$BinDirectory;$env:Path"
}

$env:CC = Join-Path $BinDirectory "clang.exe"
$env:CXX = Join-Path $BinDirectory "clang++.exe"
$env:PKG_CONFIG = Join-Path $BinDirectory "pkg-config.exe"

Write-Host "Umicom Windows development environment is ready." -ForegroundColor Green
Write-Host "UCRT64 bin: $BinDirectory"
Write-Host "CC: $env:CC"
Write-Host "CXX: $env:CXX"
Write-Host "PKG_CONFIG: $env:PKG_CONFIG"
Write-Host ""
Write-Host "Selected tools:" -ForegroundColor Cyan
where.exe clang
where.exe ninja
where.exe pkg-config
where.exe gcc
where.exe windres
