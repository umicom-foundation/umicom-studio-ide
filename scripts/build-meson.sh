#!/usr/bin/env bash
set -euo pipefail
meson setup build
meson compile -C build
./build/ustudio
