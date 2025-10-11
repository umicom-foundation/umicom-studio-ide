# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
<#
.SYNOPSIS
  Deletes duplicate files listed in a text file (one path per line).

.DESCRIPTION
  Use alongside find_dupes.py which writes tools/delete_candidates.txt.
  This script supports a "dry run" and also accepts -WhatIf as an alias
  for convenience (no relation to the common ShouldProcess switch).

.PARAMETER ListPath
  Path to a UTF-8 text file with one path per line. Lines starting with '#'
  or blank lines are ignored.

.PARAMETER DryRun
  If set, only prints what would be deleted.

.PARAMETER Force
  Skip the "Type YES to confirm" prompt.

.PARAMETER WhatIf
  Alias for -DryRun (accepted for user convenience).

.EXAMPLE
  powershell -ExecutionPolicy Bypass -File tools/cleanup_dupes.ps1 -ListPath tools/delete_candidates.txt -DryRun
  powershell -ExecutionPolicy Bypass -File tools/cleanup_dupes.ps1 -ListPath tools/delete_candidates.txt -Force
  powershell -ExecutionPolicy Bypass -File tools/cleanup_dupes.ps1 -ListPath tools/delete_candidates.txt -WhatIf
#>

[CmdletBinding()]
param(
  [Parameter(Mandatory=$true)]
  [ValidateNotNullOrEmpty()]
  [string]$ListPath,

  [Alias('WhatIf')]
  [switch]$DryRun,

  [switch]$Force
)

# Resolve to absolute path for friendliness
$ListPath = (Resolve-Path -LiteralPath $ListPath).Path 2>$null

if (-not $ListPath -or -not (Test-Path -LiteralPath $ListPath)) {
  Write-Error "ListPath not found: $ListPath"
  exit 1
}

# Read file list
$files = Get-Content -LiteralPath $ListPath -ErrorAction Stop |
  Where-Object { $_ -and ($_.Trim()).Length -gt 0 -and -not ($_.Trim().StartsWith('#')) } |
  ForEach-Object { $_.Trim() }

if ($files.Count -eq 0) {
  Write-Host "Nothing to delete. ($ListPath had no candidates)"
  exit 0
}

Write-Host "Candidate files ($($files.Count)):" -ForegroundColor Cyan
foreach ($f in $files) {
  Write-Host ("  " + $f)
}

if ($DryRun.IsPresent) {
  Write-Host "`nDry run only. No files were deleted." -ForegroundColor Yellow
  exit 0
}

if (-not $Force.IsPresent) {
  $confirm = Read-Host "`nType YES to permanently delete the files above"
  if ($confirm -ne "YES") {
    Write-Host "Aborted."
    exit 0
  }
}

$deleted = 0
$missing = 0
$failed  = 0

foreach ($f in $files) {
  try {
    if (Test-Path -LiteralPath $f) {
      Remove-Item -LiteralPath $f -Force -ErrorAction Stop
      Write-Host "Deleted: $f"
      $deleted++
    } else {
      Write-Host "Missing: $f"
      $missing++
    }
  } catch {
    Write-Warning "Failed: $f  --> $($_.Exception.Message)"
    $failed++
  }
}

Write-Host "`nSummary: deleted=$deleted, missing=$missing, failed=$failed"
exit 0
