#!/usr/bin/env bash
set -euo pipefail
DEST="${1:-$HOME/dev/umicom-studio-ide}"
mkdir -p "$DEST"/{src,data/ui,data/styles,docs,assets/mockups,.vscode}
[ -f "$DEST/README.md" ] || cp -n README.md "$DEST/README.md" || true
echo "[bootstrap] umicom-studio-ide structure ensured at $DEST"
