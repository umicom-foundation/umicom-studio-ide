#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

set -euo pipefail
ROOT="$(pwd)"
[ -f "mkdocs.yml" ] && cp mkdocs.yml mkdocs.yml.bak || true
cp -f mkdocs_nav_overlay.yml mkdocs.yml
mkdir -p docs
rsync -a --ignore-existing docs/ "${ROOT}/docs/" 2>/dev/null || true
# Always copy/overwrite the nav-linked pages from overlay
cp -rf docs/* "${ROOT}/docs/"
echo "Installed mkdocs.yml and docs/. Backup saved as mkdocs.yml.bak (if existed)."
