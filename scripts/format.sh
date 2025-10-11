#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
set -euo pipefail
if ! command -v clang-format >/dev/null; then echo "clang-format not found"; exit 0; fi
find src include -type f \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 clang-format -i
