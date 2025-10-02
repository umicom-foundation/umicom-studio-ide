# Umicom Studio IDE — developer bootstrap for Windows (PowerShell)
# Purpose: install MSYS2 toolchain, GTK, json-glib, and ripgrep; then build.
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
Param(
  [string]$Workspace = (Get-Location).Path
)

Write-Host ">>> Setting up developer environment at $Workspace"
# 1) Check MSYS2
if (-not (Test-Path "C:\msys64\usr\bin\bash.exe")) {
  Write-Host "MSYS2 not found. Please install from https://www.msys2.org/ and re-run." -ForegroundColor Yellow
  exit 1
}
# 2) Install deps
& C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-json-glib ripgrep"
# 3) Configure build (Meson)
& C:\msys64\usr\bin\bash.exe -lc "meson setup build && meson compile -C build"
Write-Host ">>> Done. Launching app..."
& C:\msys64\usr\bin\bash.exe -lc "./build/umicom-studio"
