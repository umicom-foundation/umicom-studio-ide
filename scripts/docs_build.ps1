# Build docs (PowerShell)
Param([switch]$ApiOnly)
$ErrorActionPreference = 'Stop'

# Generate Doxygen HTML into docs/reference/api
if (Get-Command doxygen -ErrorAction SilentlyContinue) {
  doxygen Doxyfile
} else {
  Write-Warning 'doxygen not found; skipping API HTML generation'
}

if ($ApiOnly) { exit 0 }

# Build MkDocs site locally
try { python -c "import mkdocs" | Out-Null } catch { pip install mkdocs mkdocs-material }
mkdocs build
