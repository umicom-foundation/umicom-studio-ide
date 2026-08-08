#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Optional Unix helper for the canonical CMake + Ninja build.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# -----------------------------------------------------------------------------
set -euo pipefail

mode="${1:-gtk}"
if [[ "$mode" == "headless" ]]; then
  cmake -S . -B build/linux-headless -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DBUILD_TESTING=ON \
    -DUMICOM_STUDIO_BUILD_GTK=OFF
  cmake --build build/linux-headless
  ctest --test-dir build/linux-headless --output-on-failure
else
  cmake -S . -B build/linux-gtk -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DBUILD_TESTING=ON \
    -DUMICOM_STUDIO_BUILD_GTK=ON
  cmake --build build/linux-gtk
  ctest --test-dir build/linux-gtk --output-on-failure
fi
