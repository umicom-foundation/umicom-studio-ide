# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
# Purpose: Repository reorganization and validation helpers.

# Lints top-level layout, warns on stray dirs/files and checks required files.
$allowedDirs = @(
  ".github",".vscode","assets","build","cmake","data","docs","include",
  "scripts","src","tasks","templates","tests","tools","uengine","patches"
)
$allowedFiles = @(
  ".clang-format",".editorconfig",".gitattributes",".gitignore",
  "CMakeLists.txt","CMakePresets.json","LICENSE","LICENSE.txt","README.md","mkdocs.yml","projects.json","Makefile","meson.build","Doxyfile"
)

$root = Get-Location
$items = Get-ChildItem -Force
$bad = @()
foreach($i in $items){
  if($i.PSIsContainer){
    if($allowedDirs -notcontains $i.Name){ $bad += "DIR: " + $i.Name }
  } else {
    if($allowedFiles -notcontains $i.Name){ $bad += "FILE: " + $i.Name }
  }
}
if($bad.Count -gt 0){
  Write-Warning "Unexpected items in repo root:"
  $bad | ForEach-Object { "  $_" }
} else {
  Write-Host "Root looks clean."
}

# Check required files exist
$req = @(".clang-format",".editorconfig","CMakeLists.txt","README.md")
foreach($f in $req){
  if(-not (Test-Path $f)){ Write-Warning "Missing required: $f" }
}

# Quick header check in templates & src: look for Umicom credit line in first 5 lines
$paths = @("src","include","templates")
foreach($p in $paths){
  if(-not (Test-Path $p)) { continue }
  Get-ChildItem $p -Recurse -File | ForEach-Object {
    $head = Get-Content $_ -TotalCount 5 -ErrorAction SilentlyContinue
    if( ($head -join "`n") -notmatch "Created by: Umicom Foundation" ){
      Write-Warning "Missing credit header: $($_.FullName)"
    }
  }
}

Write-Host "Validation complete."
