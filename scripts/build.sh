#!/usr/bin/env bash
set -euo pipefail
meson setup build -Dbuildtype=debug || meson setup build --reconfigure
meson compile -C build
[ "${1:-}" = "--test" ] && meson test -C build
[ "${1:-}" = "--run"  ] && ./build/umicom-studio || true
