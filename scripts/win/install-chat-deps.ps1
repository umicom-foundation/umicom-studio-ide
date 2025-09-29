
[CmdletBinding()]param()
$ErrorActionPreference='Stop'
$msys = "C:\msys64"
$bash = Join-Path $msys "usr\bin\bash.exe"
if(-not (Test-Path $bash)){ throw "MSYS2 not found. Run the bootstrap scripts first." }
$cmd = @"
  set -e
  export CHERE_INVOKING=1
  source /etc/profile
  export MSYSTEM=MINGW64
  pacman -S --needed --noconfirm mingw-w64-x86_64-libsoup3 mingw-w64-x86_64-json-glib
  echo "[OK] libsoup3 + json-glib installed."
"@
& $bash -lc $cmd
