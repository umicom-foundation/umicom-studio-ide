# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------

Param([string]$Dest="C:\dev\umicom-studio-ide")
$ErrorActionPreference="Stop"
if (!(Test-Path $Dest)) { New-Item -ItemType Directory -Force -Path $Dest | Out-Null }
$layout = @("src","data\ui","data\styles","docs","assets\mockups",".vscode")
foreach ($p in $layout) { New-Item -ItemType Directory -Force -Path (Join-Path $Dest $p) | Out-Null }
if (!(Test-Path (Join-Path $Dest "README.md"))) {
  Get-Content ".\README.md" | Out-File (Join-Path $Dest "README.md") -Encoding UTF8
}
Write-Host "[bootstrap] umicom-studio-ide structure ensured at $Dest"
