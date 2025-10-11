#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
set -euo pipefail
meson setup build -Dbuildtype=debug || meson setup build --reconfigure
meson compile -C build
[ "${1:-}" = "--test" ] && meson test -C build
[ "${1:-}" = "--run"  ] && ./build/umicom-studio || true
