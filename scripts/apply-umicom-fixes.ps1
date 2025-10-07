# Requires: PowerShell 5+
$ErrorActionPreference = "Stop"

function Ensure-LineInFile {
  param($Path, $Marker, $BlockToAppend)
  if (!(Test-Path $Path)) { Write-Host "Skip (missing): $Path"; return }
  $txt = Get-Content -Raw -Encoding UTF8 $Path
  if ($txt.Contains($Marker)) {
    Write-Host "Already present in $Path"
  } else {
    Add-Content -Encoding UTF8 -Path $Path -Value "`n$BlockToAppend`n"
    Write-Host "Appended addendum to $Path"
  }
}

function Replace-RegexInFile {
  param($Path, $Pattern, $Replacement)
  if (!(Test-Path $Path)) { return $false }
  $txt = Get-Content -Raw -Encoding UTF8 $Path
  $new = [System.Text.RegularExpressions.Regex]::Replace($txt, $Pattern, $Replacement, 'Singleline')
  if ($new -ne $txt) {
    Set-Content -Encoding UTF8 -NoNewline -Path $Path -Value $new
    return $true
  }
  return $false
}

function Add-Includes-If-Missing {
  param($Path, [string[]]$IncludesToEnsure)
  if (!(Test-Path $Path)) { return $false }
  $txt = Get-Content -Raw -Encoding UTF8 $Path
  $missing = @()
  foreach ($inc in $IncludesToEnsure) {
    if ($txt -notmatch [regex]::Escape($inc)) { $missing += $inc }
  }
  if ($missing.Count -eq 0) { return $false }

  # insert after the last #include line near the top
  $lines = $txt -split "`r?`n"
  $idx = ($lines | Select-String -Pattern '^\s*#\s*include\b' | Select -Last 1).LineNumber
  if (-not $idx) { $idx = 1 }
  $before = $lines[0..($idx-1)]
  $after  = $lines[$idx..($lines.Length-1)]
  $inject = $missing | ForEach-Object { '#include "' + $_ + '"' }
  $new = @($before + $inject + $after) -join "`n"
  Set-Content -Encoding UTF8 -NoNewline -Path $Path -Value $new
  return $true
}

Write-Host "== Applying Umicom Windows build fixes =="

# 1) CMake: append addendum if needed
$cmakeRoot = Join-Path (Get-Location) "CMakeLists.txt"
$addendum = Get-Content -Raw -Encoding UTF8 "patches/CMakeLists.addendum.cmake"
Ensure-LineInFile -Path $cmakeRoot -Marker "Begin auto-added by scripts/apply-umicom-fixes.ps1" -BlockToAppend $addendum

# 2) Normalise includes "uengine/" -> "ueng/" ONLY if both forms exist in tree
$hasUeng    = @(Get-ChildItem -Recurse -File -Include *.h,*.c,*.cpp,*.hpp | Select-String -Pattern '"ueng/').Count -gt 0
$hasUengine = @(Get-ChildItem -Recurse -File -Include *.h,*.c,*.cpp,*.hpp | Select-String -Pattern '"uengine/').Count -gt 0
if ($hasUeng -and $hasUengine) {
  Get-ChildItem -Recurse -File -Include *.c,*.h,*.cpp,*.hpp | ForEach-Object {
    $p = $_.FullName
    $txt = Get-Content -Raw -Encoding UTF8 $p
    $new = $txt -replace '"uengine/', '"ueng/'
    if ($new -ne $txt) { Set-Content -Encoding UTF8 -NoNewline -Path $p -Value $new }
  }
  Write-Host "Normalised uengine/ -> ueng/ where applicable"
} else {
  Write-Host "Skip normalising uengine path (only one form found)"
}

# 3) src/build/build_system.c: remove duplicate split_argv_safe (keep the first)
$buildSys = "src/build/build_system.c"
if (Test-Path $buildSys) {
  $txt = Get-Content -Raw -Encoding UTF8 $buildSys
  $pattern = 'static\s+GPtrArray\s*\*\s*split_argv_safe\s*\([^)]*\)\s*\{.*?\}'
  $matches = [System.Text.RegularExpressions.Regex]::Matches($txt, $pattern, 'Singleline')
  if ($matches.Count -gt 1) {
    $firstEnd = $matches[0].Index + $matches[0].Length
    $before = $txt.Substring(0,$firstEnd)
    $rest = $txt.Substring($firstEnd)
    $restClean = [System.Text.RegularExpressions.Regex]::Replace($rest, $pattern, "/* removed duplicate split_argv_safe */", 'Singleline')
    Set-Content -Encoding UTF8 -NoNewline -Path $buildSys -Value ($before + $restClean)
    Write-Host "Removed duplicate split_argv_safe from $buildSys"
  } else {
    Write-Host "No duplicate split_argv_safe found in $buildSys"
  }
}

# 4) src/llm/providers/llm_zai.c: fix invalid nested comment header
$zai = "src/llm/providers/llm_zai.c"
if (Test-Path $zai) {
  $fixed = Replace-RegexInFile -Path $zai -Pattern "\*\s*/\*\s*Created by:.*?\n\*\s*-{2,}.*?\*/" -Replacement "* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */"
  if ($fixed) { Write-Host "Fixed nested comment in $zai" } else { Write-Host "Comment header in $zai already OK or pattern not found" }
}

# 5) src/gui/llm_lab.c: replace removed GTK4 pending/iteration with GLib main context
$lab = "src/gui/llm_lab.c"
if (Test-Path $lab) {
  $fixed = Replace-RegexInFile -Path $lab -Pattern "while\s*\(\s*gtk_events_pending\s*\(\s*\)\s*\)\s*\)\s*\{\s*gtk_main_iteration\s*\(\s*\)\s*;\s*\}" -Replacement "while (g_main_context_pending(NULL)) { g_main_context_iteration(NULL, $false); }"
  if (-not $fixed) {
    $fixed = Replace-RegexInFile -Path $lab -Pattern "while\s*\(\s*gtk_events_pending\s*\(\s*\)\s*\)\s*\r?\n\s*gtk_main_iteration\s*\(\s*\)\s*;" -Replacement "while (g_main_context_pending(NULL)) { g_main_context_iteration(NULL, $false); }"
  }
  if ($fixed) { Write-Host "Replaced deprecated GTK loop in $lab" } else { Write-Host "GTK loop replacement not needed or pattern not found in $lab" }
}

# 6) src/gui/app.c: add missing includes for UmiApp / editor API
$appc = "src/gui/app.c"
if (Test-Path $appc) {
  $added = Add-Includes-If-Missing -Path $appc -IncludesToEnsure @("umicom/app.h","editor/editor.h")
  if ($added) { Write-Host "Injected missing includes in $appc" } else { Write-Host "Includes in $appc already OK" }
}

# 7) src/llm/llm_core.c: replace broken SSE parsing with working snippet
$core = "src/llm/llm_core.c"
if (Test-Path $core) {
  $txt = Get-Content -Raw -Encoding UTF8 $core
  $badQuotes = $txt -match "if\s*\(c=='\s*\)\s*continue;" -or $txt -match "g_strsplit\s*\(\s*sc->buf->str\s*,\s*""\s*\r?\n\s*""\s*,\s*0\s*\)"
  if ($badQuotes) {
    $snippet = Get-Content -Raw -Encoding UTF8 "patches/llm_core_sse_parser.c"
    $m1 = [System.Text.RegularExpressions.Regex]::Match($txt, "g_strsplit\s*\(\s*sc->buf->str.*?", 'Singleline')
    $m2 = [System.Text.RegularExpressions.Regex]::Match($txt, "g_strfreev\s*\(\s*lines\s*\)\s*;", 'Singleline')
    if ($m1.Success -and $m2.Success -and $m2.Index -gt $m1.Index) {
      $before = $txt.Substring(0, $m1.Index)
      $afterIdx = $m2.Index + $m2.Length
      $after = $txt.Substring($afterIdx)
      $new = $before + $snippet + $after
      Set-Content -Encoding UTF8 -NoNewline -Path $core -Value $new
      Write-Host "Replaced broken SSE parser in $core (region swap)"
    } else {
      Write-Warning "Could not confidently locate the broken SSE block in $core. Please copy patches\\llm_core_sse_parser.c manually."
    }
  } else {
    Write-Host "SSE parser in $core looks OK; no change"
  }
}

Write-Host "== Done. You can now run scripts\\verify-build.ps1 to configure & build =="
