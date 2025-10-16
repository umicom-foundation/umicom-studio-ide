# scripts\tools\Make-UmicomAssets.ps1
$ErrorActionPreference = 'Stop'
$root   = Split-Path -Parent $MyInvocation.MyCommand.Path
$proj   = Resolve-Path (Join-Path $root '..')
$assets = Join-Path $proj 'assets'
$win    = Join-Path $proj 'win'

New-Item -ItemType Directory -Force -Path $win | Out-Null

$ico = Join-Path $win 'umicom.ico'
$png = Join-Path $win 'umicom-logo.png'

# Remove stale outputs first (avoids “open” issues)
Remove-Item -Force -ErrorAction SilentlyContinue $ico, $png

# Build ICO from SVG (multi-size), keep alpha; keep it quiet
magick -quiet -background none -density 1024 `
  (Join-Path $assets 'umicom-icon.svg') `
  -define "icon:auto-resize=256,128,96,64,48,32,24,20,16" `
  $ico

# Build a header/logo PNG at ~800 px width
magick -quiet -background none -density 512 `
  (Join-Path $assets 'umicom-logo.svg') `
  -resize 800 `
  $png

Write-Host "Generated: $ico"
Write-Host "Generated: $png"
