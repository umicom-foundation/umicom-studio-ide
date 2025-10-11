# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Commit helper (PowerShell)
Param([string]$Message = "umicom: overlay apply (commit helper)")
git status
git add -A
git status
git commit -m "$Message"
git push origin HEAD
