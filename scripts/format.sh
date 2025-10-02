#!/usr/bin/env bash
set -euo pipefail
if ! command -v clang-format >/dev/null; then echo "clang-format not found"; exit 0; fi
find src include -type f \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 clang-format -i
