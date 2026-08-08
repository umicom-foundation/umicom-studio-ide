#!/usr/bin/env bash
# Umicom Studio IDE — Unix dependency setup and canonical CMake build
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
set -euo pipefail

if command -v apt >/dev/null 2>&1; then
  sudo apt update
  sudo apt install -y clang cmake ninja-build pkg-config libgtk-4-dev \
    libgtksourceview-5-dev libjson-glib-dev libsoup-3.0-dev libcurl4-openssl-dev \
    libsqlite3-dev ripgrep
elif command -v dnf >/dev/null 2>&1; then
  sudo dnf install -y clang cmake ninja-build pkgconf-pkg-config gtk4-devel \
    gtksourceview5-devel json-glib-devel libsoup3-devel libcurl-devel \
    sqlite-devel ripgrep
elif command -v brew >/dev/null 2>&1; then
  brew install llvm cmake ninja pkg-config gtk4 gtksourceview5 json-glib \
    libsoup curl sqlite ripgrep
else
  echo "Unsupported package manager. Install Clang, CMake, Ninja, GTK4 and dependencies manually."
  exit 1
fi

git submodule update --init --recursive
cmake -S . -B build/linux-gtk -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DBUILD_TESTING=ON \
  -DUMICOM_STUDIO_BUILD_GTK=ON
cmake --build build/linux-gtk
ctest --test-dir build/linux-gtk --output-on-failure
