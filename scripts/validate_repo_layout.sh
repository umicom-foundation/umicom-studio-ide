#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
# Purpose: Repository reorganization and validation helpers.

set -euo pipefail
ALLOWED_DIRS=".github .vscode assets build cmake data docs include scripts src tasks templates tests tools uengine patches"
ALLOWED_FILES=".clang-format .editorconfig .gitattributes .gitignore CMakeLists.txt CMakePresets.json LICENSE LICENSE.txt README.md mkdocs.yml projects.json Makefile meson.build Doxyfile"

BAD=0
for item in .* *; do
  [ "$item" = "." ] || [ "$item" = ".." ] || true
  if [ -d "$item" ]; then
    echo "$ALLOWED_DIRS" | grep -q -w "$item" || { echo "WARN: unexpected DIR: $item"; BAD=1; }
  elif [ -f "$item" ]; then
    echo "$ALLOWED_FILES" | grep -q -w "$item" || { echo "WARN: unexpected FILE: $item"; BAD=1; }
  fi
done

for f in .clang-format .editorconfig CMakeLists.txt README.md; do
  [ -f "$f" ] || { echo "WARN: missing required $f"; BAD=1; }
done

# Header check
for base in src include templates; do
  [ -d "$base" ] || continue
  while IFS= read -r -d '' file; do
    head -n 5 "$file" | grep -q "Created by: Umicom Foundation" || echo "WARN: missing credit header: $file"
  done < <(find "$base" -type f -print0)
done

echo "Validation complete."
exit 0
