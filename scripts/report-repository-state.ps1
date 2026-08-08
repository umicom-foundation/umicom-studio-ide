# Umicom Studio IDE repository-state report (read only)
# Author: Sammy Hegab | Umicom Foundation | MIT
$ErrorActionPreference = "Stop"
Write-Host "Studio branch and status" -ForegroundColor Cyan
git branch --show-current
git status --short
Write-Host "Submodule commits" -ForegroundColor Cyan
git submodule status --recursive
Write-Host "Framework status" -ForegroundColor Cyan
git -C framework status --short
Write-Host "AuthorEngine status" -ForegroundColor Cyan
git -C third_party/umicom/uengine status --short
Write-Host "llama.cpp status" -ForegroundColor Cyan
git -C third_party/umicom/uengine/third_party/llama.cpp status --short
