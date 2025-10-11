# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
<#-----------------------------------------------------------------------------
 PURPOSE: Identify source files in src/ that are NOT part of the current build.
 HOW: Generates compile_commands.json via CMake, reads it, compares to
      all .c/.cpp under src/. Outputs a list for manual review.
 USAGE:
   PS> Set-Location C:\dev\umicom-studio-ide
   PS> .\scripts\reports\report-orphan-sources.ps1
 -----------------------------------------------------------------------------#>
[CmdletBinding()] param()

$Root = Get-Location
$BuildDir = Join-Path $Root "build-ninja"
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

# Configure with compile commands
cmake -S $Root -B $BuildDir -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON | Out-Null

$ccdb = Join-Path $BuildDir "compile_commands.json"
if (!(Test-Path $ccdb)) { Write-Error "Missing $ccdb"; exit 1 }

$compiled = Get-Content $ccdb -Raw | ConvertFrom-Json | ForEach-Object { $_.file } |
  ForEach-Object { $_.Replace('/', '\') } |
  ForEach-Object { if ($_ -like "$($Root)\*") { $_.Substring($Root.Path.Length+1) } else { $_ } } |
  ForEach-Object { $_.ToLower() } | Sort-Object -Unique

$srcFiles = Get-ChildItem -Recurse -File src -Include *.c,*.cpp |
  ForEach-Object { $_.FullName.Substring($Root.Path.Length+1).ToLower() } |
  Sort-Object -Unique

$orphans = Compare-Object -ReferenceObject $srcFiles -DifferenceObject $compiled -PassThru |
  Where-Object { $_ -in $srcFiles } | Sort-Object

$report = Join-Path $Root "reports\orphans_sources.txt"
New-Item -ItemType Directory -Force -Path (Split-Path $report) | Out-Null
$orphans | Set-Content -Encoding UTF8 $report

Write-Host "Orphan sources: $($orphans.Count). See $report"
# --- IGNORE ---