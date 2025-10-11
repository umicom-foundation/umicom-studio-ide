<#-----------------------------------------------------------------------------
 PURPOSE: List headers outside include/ (excluding third_party, tests).
         Helps decide what should become public API (move to include/)
         vs private (kept near sources).
 USAGE:
   PS> Set-Location C:\dev\umicom-studio-ide
   PS> .\scripts\reports\report-stray-headers.ps1
 -----------------------------------------------------------------------------#>
[CmdletBinding()] param()

$Root = Get-Location
$headers = Get-ChildItem -Recurse -File -Include *.h,*.hpp |
  Where-Object {
    $_.FullName -notmatch "\\include\\" -and
    $_.FullName -notmatch "\\third_party\\" -and
    $_.FullName -notmatch "\\tests\\"
  } |
  ForEach-Object { $_.FullName.Substring($Root.Path.Length+1) } |
  Sort-Object

$report = Join-Path $Root "reports\headers_outside_include.txt"
New-Item -ItemType Directory -Force -Path (Split-Path $report) | Out-Null
$headers | Set-Content -Encoding UTF8 $report

Write-Host "Headers outside include/: $($headers.Count). See $report"
# --- IGNORE ---