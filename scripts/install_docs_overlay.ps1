# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

param()
if (Test-Path "mkdocs.yml") { Copy-Item mkdocs.yml mkdocs.yml.bak -Force }
Copy-Item mkdocs_nav_overlay.yml mkdocs.yml -Force
New-Item -ItemType Directory -Force -Path docs | Out-Null
# Copy only missing files first
robocopy docs .\docs /E /NFL /NDL /NJH /NJS /XO | Out-Null
# Then force-copy nav-linked pages to ensure consistency
robocopy docs .\docs /E /NFL /NDL /NJH /NJS /IS | Out-Null
Write-Host "Installed mkdocs.yml and docs/. Backup saved as mkdocs.yml.bak (if existed)."
