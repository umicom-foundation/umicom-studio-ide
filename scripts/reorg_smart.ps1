<#  -----------------------------------------------------------------------------
  Umicom Studio IDE — Smart Reorg (PowerShell)  v1.2
  - Moves Android/Web/Wt starters into templates/* and tidies single files.
  - Uses `git mv` for tracked files; falls back to Move-Item for untracked.
  - Never overwrites (same-size -> skip; diff-size -> conflict).
  - Moves directories by iterating files (avoids “source directory is empty”).
  - Supports -DryRun to preview.
  - Author: Umicom Foundation | Sammy Hegab | MIT
----------------------------------------------------------------------------- #>

[CmdletBinding()]
param(
    [switch]$DryRun = $false,
    [switch]$VerboseLog = $true,
    [switch]$RemoveEmptySources = $true
)

function Write-Info { param([string]$m) if ($VerboseLog) { Write-Host "[INFO ] $m" -ForegroundColor Cyan } }
function Write-Warn { param([string]$m) Write-Host "[WARN ] $m" -ForegroundColor Yellow }
function Write-ErrMsg { param([string]$m) Write-Host "[ERROR] $m" -ForegroundColor Red }

function Ensure-Dir([string]$Path) {
    if ([string]::IsNullOrWhiteSpace($Path)) { return }
    if (-not (Test-Path -LiteralPath $Path)) {
        if ($DryRun) { Write-Info "DRY: mkdir $Path" }
        else { New-Item -ItemType Directory -Force -Path $Path | Out-Null }
    }
}

function Test-GitRepo() { return (Test-Path -LiteralPath ".git") }

function Test-GitTracked([string]$Path) {
    if (-not (Test-GitRepo)) { return $false }
    $null = & git ls-files --error-unmatch -- "$Path" 2>$null
    return ($LASTEXITCODE -eq 0)
}

# Safe parent dir detector (avoids Split-Path ambiguity)
function Get-ParentDir([string]$Path) {
    if ([string]::IsNullOrWhiteSpace($Path)) { return $null }
    try {
        $p = [System.IO.Path]::GetDirectoryName($Path)
        if ([string]::IsNullOrWhiteSpace($p)) { return $null }
        return $p
    }
    catch {
        return $null
    }
}

# Global counters
$script:MovedCount = 0
$script:SameCount = 0
$script:ConflictCount = 0
$script:MissingList = New-Object System.Collections.ArrayList
$script:SameList = New-Object System.Collections.ArrayList
$script:ConflictList = New-Object System.Collections.ArrayList

# Move one file; never overwrite existing files
function Move-FileSmart([string]$SrcFile, [string]$DstFile) {
    if (-not (Test-Path -LiteralPath $SrcFile)) {
        Write-Warn "Missing: $SrcFile"
        [void]$script:MissingList.Add($SrcFile)
        return
    }

    $dstDir = Get-ParentDir $DstFile
    Ensure-Dir $dstDir

    $srcInfo = Get-Item -LiteralPath $SrcFile -ErrorAction SilentlyContinue
    if (-not $srcInfo) {
        Write-Warn "Cannot stat: $SrcFile"
        return
    }

    if (Test-Path -LiteralPath $DstFile) {
        $dstInfo = Get-Item -LiteralPath $DstFile -ErrorAction SilentlyContinue
        if ($dstInfo -and $dstInfo.PSIsContainer) {
            Write-Warn "CONFLICT (dest is a directory): `"$DstFile`"  — skipping file `"$SrcFile`""
            [void]$script:ConflictList.Add("$SrcFile -> $DstFile (dest is dir)")
            $script:ConflictCount++
            return
        }
        if ($dstInfo -and $dstInfo.Length -eq $srcInfo.Length) {
            Write-Info "Skip same-size: `"$SrcFile`" -> `"$DstFile`""
            [void]$script:SameList.Add("$SrcFile -> $DstFile")
            $script:SameCount++
            return
        }
        else {
            Write-Warn "CONFLICT (different size): `"$DstFile`"  — skipping `"$SrcFile`""
            [void]$script:ConflictList.Add("$SrcFile -> $DstFile")
            $script:ConflictCount++
            return
        }
    }

    if ($DryRun) {
        if (Test-GitTracked $SrcFile) {
            Write-Info "DRY git mv `"$SrcFile`" `"$DstFile`""
        }
        else {
            Write-Info "DRY move `"$SrcFile`" `"$DstFile`""
        }
        return
    }

    if (Test-GitTracked $SrcFile) {
        & git mv -- "$SrcFile" "$DstFile"
        if ($LASTEXITCODE -ne 0) {
            Write-Warn "git mv failed; fallback to filesystem move: $SrcFile"
            Move-Item -LiteralPath $SrcFile -Destination $DstFile -Force:$false
        }
    }
    else {
        Move-Item -LiteralPath $SrcFile -Destination $DstFile -Force:$false
    }

    $script:MovedCount++
}

# Move directory by iterating files (handles empty/untracked dirs)
function Move-DirSmart([string]$SrcDir, [string]$DstDir) {
    if (-not (Test-Path -LiteralPath $SrcDir)) {
        Write-Warn "Missing dir: $SrcDir"
        [void]$script:MissingList.Add($SrcDir)
        return
    }

    Ensure-Dir $DstDir

    $srcRootItem = Get-Item -LiteralPath $SrcDir -ErrorAction SilentlyContinue
    if (-not $srcRootItem) {
        Write-Warn "Cannot stat dir: $SrcDir"
        return
    }
    $srcRootPath = $srcRootItem.FullName.TrimEnd('\', '/')

    $files = Get-ChildItem -LiteralPath $SrcDir -File -Recurse -ErrorAction SilentlyContinue
    if (-not $files) {
        Write-Info "Empty or no files in: $SrcDir (nothing to move)"
    }
    else {
        foreach ($f in $files) {
            $full = $f.FullName
            $rel = $full.Substring($srcRootPath.Length).TrimStart('\', '/')
            $target = Join-Path $DstDir $rel
            Ensure-Dir (Get-ParentDir $target)
            Move-FileSmart -SrcFile $full -DstFile $target
        }
    }

    if ($RemoveEmptySources -and -not $DryRun) {
        try {
            $remaining = Get-ChildItem -LiteralPath $SrcDir -Recurse -Force -ErrorAction SilentlyContinue
            if (-not $remaining) { Remove-Item -LiteralPath $SrcDir -Recurse -Force }
        }
        catch {
            Write-Warn "Could not remove dir (maybe not empty): $SrcDir"
        }
    }
    elseif ($DryRun) {
        Write-Info "DRY: would remove dir if empty: $SrcDir"
    }
}

# ------------------------ mapping ------------------------
$Moves = @(
    # Android → templates/android/full-mm
    @{ Src = "app"; Dst = "templates/android/full-mm/app" },
    @{ Src = "feature"; Dst = "templates/android/full-mm/feature" },
    @{ Src = "lib"; Dst = "templates/android/full-mm/lib" },
    @{ Src = "gradle"; Dst = "templates/android/full-mm/gradle" },
    @{ Src = "gradlew"; Dst = "templates/android/full-mm/gradlew" },
    @{ Src = "gradlew.bat"; Dst = "templates/android/full-mm/gradlew.bat" },
    @{ Src = "build.gradle.kts"; Dst = "templates/android/full-mm/build.gradle.kts" },
    @{ Src = "gradle.properties"; Dst = "templates/android/full-mm/gradle.properties" },
    @{ Src = "settings.gradle.kts"; Dst = "templates/android/full-mm/settings.gradle.kts" },

    # Web → templates/web
    @{ Src = "web-vanilla"; Dst = "templates/web/vanilla" },
    @{ Src = "react-vite"; Dst = "templates/web/react-vite" },
    @{ Src = "react-vite-ts"; Dst = "templates/web/react-vite-ts" },
    @{ Src = "react-vite-pro"; Dst = "templates/web/react-vite-pro" },
    @{ Src = "vue-vite"; Dst = "templates/web/vue-vite" },
    @{ Src = "vue-vite-ts"; Dst = "templates/web/vue-vite-ts" },
    @{ Src = "vue-vite-pro"; Dst = "templates/web/vue-vite-pro" },
    @{ Src = "svelte-vite"; Dst = "templates/web/svelte-vite" },
    @{ Src = "sveltekit"; Dst = "templates/web/sveltekit" },
    @{ Src = "sveltekit-auth"; Dst = "templates/web/sveltekit-auth" },
    @{ Src = "node-express-api"; Dst = "templates/web/node/express-api" },
    @{ Src = "node-express-advanced"; Dst = "templates/web/node/express-advanced" },
    @{ Src = "node-express-jwt"; Dst = "templates/web/node/express-jwt" },

    # Wt demos → templates/wt
    @{ Src = "wt-hello"; Dst = "templates/wt/hello" },
    @{ Src = "wt-crud"; Dst = "templates/wt/crud" },
    @{ Src = "wt-crud-sqlite"; Dst = "templates/wt/crud-sqlite" },
    @{ Src = "wt-routing"; Dst = "templates/wt/routing" },
    @{ Src = "wt-auth-stub"; Dst = "templates/wt/auth-stub" },
    @{ Src = "wt-auth-hash"; Dst = "templates/wt/auth-hash" },
    @{ Src = "wt-rest-json"; Dst = "templates/wt/rest-json" },
    @{ Src = "wt-rest-jwt-stub"; Dst = "templates/wt/rest-jwt-stub" },
    @{ Src = "wt-file-upload"; Dst = "templates/wt/file-upload" },
    @{ Src = "wt-charts"; Dst = "templates/wt/charts" },

    # Single-file tidy
    @{ Src = "CMakeLists.overlay.txt"; Dst = "cmake/CMakeLists.overlay.txt" },
    @{ Src = "Doxyfile"; Dst = "docs/Doxyfile" },
    @{ Src = "INSTALL_PART34.md"; Dst = "docs/releases/INSTALL_PART34.md" },
    @{ Src = "MANIFEST_PART34.json"; Dst = "docs/releases/MANIFEST_PART34.json" },
    @{ Src = "README-Codestral.md"; Dst = "docs/reference/README-Codestral.md" }
)

# Ensure destination roots exist
@(
    "templates/android/full-mm",
    "templates/web",
    "templates/web/node",
    "templates/wt",
    "cmake",
    "docs/releases",
    "docs/reference"
) | ForEach-Object { Ensure-Dir $_ }

# ------------------------ execute ------------------------
foreach ($m in $Moves) {
    $src = $m.Src
    $dst = $m.Dst

    if (-not (Test-Path -LiteralPath $src)) {
        Write-Warn "Not found: $src"
        [void]$script:MissingList.Add($src)
        continue
    }

    $srcItem = Get-Item -LiteralPath $src -ErrorAction SilentlyContinue
    if (-not $srcItem) {
        Write-Warn "Cannot stat path: $src"
        [void]$script:MissingList.Add($src)
        continue
    }

    if ($srcItem.PSIsContainer) {
        Write-Info "Dir move: $src -> $dst"
        Move-DirSmart -SrcDir $src -DstDir $dst
    }
    else {
        Move-FileSmart -SrcFile $src -DstFile $dst
    }
}

# ------------------------ summary ------------------------
Write-Host ""
Write-Host "======== Summary ========" -ForegroundColor Green
Write-Host ("Moved files:        {0}" -f $script:MovedCount)
Write-Host ("Same-size skipped:  {0}" -f $script:SameCount)
Write-Host ("Conflicts:          {0}" -f $script:ConflictCount)
Write-Host ("Missing sources:    {0}" -f $($script:MissingList.Count))
if ($script:SameList.Count) { Write-Host "`nSame-size:"; $script:SameList | ForEach-Object { "  $_" } }
if ($script:ConflictList.Count) { Write-Host "`nConflicts:"; $script:ConflictList | ForEach-Object { "  $_" } }
if ($script:MissingList.Count) { Write-Host "`nMissing:"; $script:MissingList | ForEach-Object { "  $_" } }
Write-Host "=========================" -ForegroundColor Green
