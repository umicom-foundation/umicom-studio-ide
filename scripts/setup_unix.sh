#!/usr/bin/env bash
# Umicom Studio IDE — developer bootstrap for Linux/macOS (bash)
# Purpose: install packages and build with Meson/Ninja.
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
set -euo pipefail
echo ">>> Installing deps (glib-2.0, gio-2.0, gtk4, json-glib-1.0, ripgrep)"
if command -v apt >/dev/null 2>&1; then
  sudo apt update && sudo apt install -y libgtk-4-dev libjson-glib-dev libglib2.0-dev ripgrep meson ninja-build
elif command -v dnf >/dev/null 2>&1; then
  sudo dnf install -y gtk4-devel json-glib-devel glib2-devel ripgrep meson ninja-build
elif command -v brew >/dev/null 2>&1; then
  brew install gtk4 json-glib glib ripgrep meson ninja
fi
echo ">>> Configuring (Meson)"
meson setup build || meson setup build --reconfigure
echo ">>> Compiling"
meson compile -C build
echo ">>> Running"
./build/umicom-studio || true
