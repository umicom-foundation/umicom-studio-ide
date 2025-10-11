<#-----------------------------------------------------------------------------
 Umicom Studio IDE
 File: scripts/cleanup/find-and-dedupe.ps1
 PURPOSE: Identify exact duplicate files by content hash and (optionally) delete
          non-canonical copies to keep the tree lean. Safe by default (WhatIf).
 Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-11 | MIT

 HOW IT WORKS
 - Scans for text/code files (.c/.h/.cpp/.json/.xml/.ps1/.cmake/.md, etc.).
 - Skips obvious vendor/test/template dirs (adjust ExcludeDirs below).
 - Groups by SHA-256; in each group, keeps the best-ranked path:
      * prefer src/, include/  (+100)
      * de-prefer third_party  (-5)
      * de-prefer backup/tmp/old (-50)
      * prefer newer timestamp on ties
 - Writes:
      dupe_reports\duplicates.csv
      dupe_reports\to_delete.txt
 - Then previews deletions with -WhatIf. Remove -WhatIf to apply.

 USAGE
   PS> Set-Location C:\dev\umicom-studio-ide
   PS> .\scripts\cleanup\find-and-dedupe.ps1
 -----------------------------------------------------------------------------#>

[CmdletBinding()]
param(
  [string[]] $Extensions = @(
    ".c",".h",".hpp",".hh",".hxx",".inl",
    ".cpp",".cxx",".cc",
    ".ui",".xml",".json",".yml",".yaml",".toml",".ini",
    ".md",".txt",".csv",".tsv",
    ".ps1",".psm1",".bat",".cmd",".sh",".bash",".zsh",
    ".py",".rb",".pl",".lua",".tcl",
    ".cmake",".meson",".ninja",".mak",".make",".mk",
    ".html",".htm",".css",".js",".ts",
    ".svg",".rst",".adoc",".desktop",
    ".clang-format",".editorconfig",".gitattributes",".gitignore"
  ),
  [string[]] $ExcludeDirs = @("third_party","tests","templates",".git",".vscode",".github","secrets",".gradle","build","dist","out")
)

$Root = Get-Location
$reportDir = Join-Path $Root "dupe_reports"
New-Item -ItemType Directory -Force -Path $reportDir | Out-Null
$csvPath = Join-Path $reportDir "duplicates.csv"
$delPath = Join-Path $reportDir "to_delete.txt"

# Gather files
$files = Get-ChildItem -Recurse -File | Where-Object {
  $okExt = $Extensions -contains ([IO.Path]::GetExtension($_.Name).ToLower())
  $inExcl = $false
  foreach ($d in $ExcludeDirs) { if ($_.FullName -match [regex]::Escape([IO.Path]::DirectorySeparatorChar+$d+[IO.Path]::DirectorySeparatorChar)) { $inExcl = $true; break } }
  $okExt -and -not $inExcl
}

if (-not $files) { Write-Host "No candidate files found."; exit 0 }

# Hash them
$rows = foreach ($f in $files) {
  try {
    $hash = Get-FileHash -Algorithm SHA256 -LiteralPath $f.FullName
    [pscustomobject]@{
      Path = $f.FullName.Substring($Root.Path.Length+1)
      Size = $f.Length
      Modified = $f.LastWriteTimeUtc
      Hash = $hash.Hash
    }
  } catch {
    Write-Warning "Hash failed: $($f.FullName)"
  }
}

# Group by hash; pick canonical
$rank = {
  param($p)
  $pl = $p.ToLower()
  $score = 0
  if ($pl -match "([\\/])src([\\/])")     { $score += 100 }
  if ($pl -match "([\\/])include([\\/])") { $score += 100 }
  if ($pl -match "third_party")           { $score -= 5   }
  if ($pl -match "backup|bak|\.orig|\.old|tmp|temp|draft"){ $score -= 50 }
  # Prefer shorter path slightly
  $score -= ($pl.Length * 0.001)
  return $score
}

$toDelete = @()
$dupCsv = @()

$rows | Group-Object Hash | Where-Object { $_.Count -gt 1 } | ForEach-Object {
  $g = $_.Group | Sort-Object @{Expression={ & $rank $_.Path }; Descending=$true }, Modified -Descending
  $keep = $g[0].Path
  foreach ($r in $g) {
    $action = if ($r.Path -eq $keep) { "Keep" } else { "Delete" }
    $dupCsv += [pscustomobject]@{ Hash=$r.Hash; Size=$r.Size; Modified=$r.Modified; Action=$action; Path=$r.Path }
    if ($action -eq "Delete") { $toDelete += $r.Path }
  }
}

$dupCsv | Sort-Object Hash, Path | Export-Csv -NoTypeInformation -Encoding UTF8 -Path $csvPath
$toDelete | Set-Content -Encoding UTF8 -Path $delPath

Write-Host "Report: $csvPath"
Write-Host "Manifest: $delPath"

# Preview deletions
foreach ($p in $toDelete) {
  if (Test-Path $p) { Remove-Item -LiteralPath $p -Force -WhatIf }
}
Write-Host "Preview done. Re-run without -WhatIf to apply deletions (edit manifest first if needed)." -ForegroundColor Yellow
# --- IGNORE ---