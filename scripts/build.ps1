# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Optional Windows helper for the canonical CMake + Ninja build.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# -----------------------------------------------------------------------------
param(
    [switch]$Headless,
    [switch]$Test,
    [switch]$Run
)

$ErrorActionPreference = "Stop"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

$preset = if ($Headless) { "headless-debug" } else { "windows-ucrt64-debug" }

cmake --preset $preset
cmake --build --preset $preset

if ($Test) {
    ctest --preset $preset
}

if ($Run) {
    if ($Headless) {
        & ".\build\headless-debug\bin\umicom-studio-console.exe"
    } else {
        & ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
    }
}
