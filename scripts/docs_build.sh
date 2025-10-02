#!/usr/bin/env bash
set -euo pipefail

API_ONLY=false
if [[ "${1:-}" == "--api" ]]; then API_ONLY=true; fi

# Generate Doxygen HTML into docs/reference/api
if command -v doxygen >/dev/null 2>&1; then
  doxygen Doxyfile
else
  echo "doxygen not found; skipping API HTML generation" >&2
fi

if [[ "$API_ONLY" == "true" ]]; then
  exit 0
fi

# Build MkDocs site locally
if ! python3 -c "import mkdocs" >/dev/null 2>&1; then
  pip install mkdocs mkdocs-material
fi
mkdocs build
