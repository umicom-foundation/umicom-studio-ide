
<#
  Install GTK4 + toolchain/meson/ninja/pkgconf inside MSYS2 MINGW64.
#>
[CmdletBinding()]param()
$ErrorActionPreference='Stop'
$msys = "C:\msys64"
$bash = Join-Path $msys "usr\bin\bash.exe"
if(-not (Test-Path $bash)){ throw "MSYS2 bash not found at $bash. Run win-bootstrap-msys2.ps1 first." }

$cmd = @"
  set -e
  export CHERE_INVOKING=1
  # Use MINGW64 environment
  source /etc/profile
  export MSYSTEM=MINGW64
  pacman -Syu --noconfirm
  pacman -S --needed --noconfirm         mingw-w64-x86_64-toolchain         mingw-w64-x86_64-gtk4         mingw-w64-x86_64-meson         mingw-w64-x86_64-ninja         mingw-w64-x86_64-pkgconf
  echo "[OK] GTK4 + toolchain installed."
"@

& $bash -lc $cmd
