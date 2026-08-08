# Umicom Studio IDE — Windows dependency check and canonical build
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
param([switch]$Run)

$ErrorActionPreference = "Stop"
$ucrt = "C:\msys64\ucrt64\bin"
if (-not (Test-Path "$ucrt\clang.exe")) {
    throw "MSYS2 UCRT64 Clang was not found. Install MSYS2 UCRT64 dependencies first."
}

$env:Path = "$ucrt;$env:Path"

git submodule update --init --recursive
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug

if ($Run) {
    & ".\build\windows-ucrt64-debug\bin\umicom-studio-ide.exe" --console
}
