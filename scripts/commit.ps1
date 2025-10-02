# Commit helper (PowerShell)
Param([string]$Message = "umicom: overlay apply (commit helper)")
git status
git add -A
git status
git commit -m "$Message"
git push origin HEAD
