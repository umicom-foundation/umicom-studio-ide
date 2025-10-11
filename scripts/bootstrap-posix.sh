#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
set -euo pipefail
DEST="${1:-$HOME/dev/umicom-studio-ide}"
mkdir -p "$DEST"/{src,data/ui,data/styles,docs,assets/mockups,.vscode}
[ -f "$DEST/README.md" ] || cp -n README.md "$DEST/README.md" || true
echo "[bootstrap] umicom-studio-ide structure ensured at $DEST"
