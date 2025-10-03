# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
# Purpose: Repository reorganization and validation helpers.

param(
  [switch]$DryRun = $false,
  [ValidateSet('SkipIfDestExists','OverwriteDest','MoveIntoTimestamped')]
  [string]$Strategy = 'SkipIfDestExists'
)

function New-DirSafe([string]$Path){
  if(-not (Test-Path $Path)){ New-Item -ItemType Directory -Force -Path $Path | Out-Null }
}

function Move-Git([string]$Src, [string]$Dst){
  if(-not (Test-Path $Src)){ Write-Host "Skip (missing): $Src"; return }
  $dstDir = Split-Path -Parent $Dst
  New-DirSafe $dstDir

  if(Test-Path $Dst){
    switch($Strategy){
      'SkipIfDestExists' {
        Write-Warning "Skip (dest exists): $Src -> $Dst"
        return
      }
      'OverwriteDest' {
        if($DryRun){ Write-Host "[DRY] git mv $Src $Dst (overwrite)"; return }
        git rm -r --cached --ignore-unmatch -- "$Dst" | Out-Null  # remove from index if tracked
        Remove-Item -Recurse -Force "$Dst"
      }
      'MoveIntoTimestamped' {
        $stamp = Get-Date -Format "yyyyMMdd_HHmmss"
        $Dst = Join-Path $dstDir ("incoming_" + $stamp + "_" + (Split-Path -Leaf $Src))
        New-DirSafe (Split-Path -Parent $Dst)
      }
    }
  }

  if($DryRun){ Write-Host "[DRY] git mv $Src $Dst"; return }
  git mv "$Src" "$Dst"
  Write-Host "Moved $Src -> $Dst"
}

# Ensure template targets exist
$targets = @(
  "templates/android/full-mm",
  "templates/android/enterprise",
  "templates/web/react-vite",
  "templates/web/react-vite-ts",
  "templates/web/vue-vite",
  "templates/web/vue-vite-ts",
  "templates/web/svelte-vite",
  "templates/web/sveltekit",
  "templates/web/node/express-api",
  "templates/web/node/express-jwt",
  "templates/web/vanilla",
  "templates/wt/hello",
  "templates/wt/crud",
  "templates/wt/crud-sqlite",
  "templates/wt/routing",
  "templates/wt/auth-stub",
  "templates/wt/rest-json",
  "templates/wt/file-upload",
  "templates/wt/charts"
)
$targets | ForEach-Object { New-DirSafe $_ }

# Android full-mm (root Android scaffolding)
Move-Git "app" "templates/android/full-mm/app"
Move-Git "feature" "templates/android/full-mm/feature"
Move-Git "lib" "templates/android/full-mm/lib"
Move-Git "gradle" "templates/android/full-mm/gradle"
Move-Git "gradlew" "templates/android/full-mm/gradlew"
Move-Git "gradlew.bat" "templates/android/full-mm/gradlew.bat"
Move-Git "build.gradle.kts" "templates/android/full-mm/build.gradle.kts"
Move-Git "gradle.properties" "templates/android/full-mm/gradle.properties"

# Web stacks
Move-Git "react-vite" "templates/web/react-vite"
Move-Git "react-vite-ts" "templates/web/react-vite-ts"
Move-Git "vue-vite" "templates/web/vue-vite"
Move-Git "vue-vite-ts" "templates/web/vue-vite-ts"
Move-Git "svelte-vite" "templates/web/svelte-vite"
Move-Git "sveltekit" "templates/web/sveltekit"
Move-Git "node-express-api" "templates/web/node/express-api"
Move-Git "node-express-advanced" "templates/web/node/express-jwt"
Move-Git "web-vanilla" "templates/web/vanilla"

# Wt demos
Move-Git "wt-hello" "templates/wt/hello"
Move-Git "wt-crud" "templates/wt/crud"
Move-Git "wt-crud-sqlite" "templates/wt/crud-sqlite"
Move-Git "wt-routing" "templates/wt/routing"
Move-Git "wt-auth-stub" "templates/wt/auth-stub"
Move-Git "wt-rest-json" "templates/wt/rest-json"
Move-Git "wt-file-upload" "templates/wt/file-upload"
Move-Git "wt-charts" "templates/wt/charts"

Write-Host "Done. Review changes with: git status"
