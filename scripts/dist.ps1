# dist.ps1 — bundle ustudio.exe with all runtime deps on Windows (GTK4 / MSYS2)
$ErrorActionPreference = "Stop"

$msys    = "C:\msys64\mingw64"       # adjust if MSYS2 lives elsewhere
$exe     = "build-ninja\ustudio.exe" # your built exe
$dist    = "dist\ustudio"

# tools
$ntldd   = Join-Path $msys "bin\ntldd.exe"
$gschemas = Join-Path $msys "bin\glib-compile-schemas.exe"
$pixbufq  = Join-Path $msys "bin\gdk-pixbuf-query-loaders.exe"

# fresh dist layout
Remove-Item $dist -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $dist | Out-Null

# app
Copy-Item $exe (Join-Path $dist "ustudio.exe")

# ----- DLLs your exe needs -----
$deps = & $ntldd -R $exe |
  Select-String '=> ' |
  ForEach-Object {
    ($_ -split '=>')[1].Trim() -replace '\(0x[0-9a-fA-F]+\)',''
  } |
  ForEach-Object { $_.Trim() } |
  Where-Object   { $_ -match '^[A-Za-z]:' } |
  Sort-Object -Unique

foreach ($d in $deps) {
  Copy-Item $d $dist -Force
}

# ----- GDK-Pixbuf image loaders (common runtime need) -----
Copy-Item -Recurse (Join-Path $msys "lib\gdk-pixbuf-2.0") (Join-Path $dist "lib\gdk-pixbuf-2.0")
& $pixbufq > (Join-Path $dist "lib\gdk-pixbuf-2.0\2.10.0\loaders.cache")

# ----- GLib schemas (recommended) -----
New-Item -ItemType Directory -Force (Join-Path $dist "share\glib-2.0\schemas") | Out-Null
Copy-Item -Recurse (Join-Path $msys "share\glib-2.0\schemas\*.xml") (Join-Path $dist "share\glib-2.0\schemas")
& $gschemas (Join-Path $dist "share\glib-2.0\schemas")

# (Optional) Icons and GTK resources – helps visuals/themes
Copy-Item -Recurse (Join-Path $msys "share\icons")   (Join-Path $dist "share\icons")   -ErrorAction SilentlyContinue
Copy-Item -Recurse (Join-Path $msys "share\gtk-4.0") (Join-Path $dist "share\gtk-4.0") -ErrorAction SilentlyContinue

Write-Host "Bundled to: $dist"
Write-Host "Run: $dist\ustudio.exe"
