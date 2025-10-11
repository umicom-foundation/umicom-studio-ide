#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
set -euo pipefail
cmake --preset default || (cmake -S . -B build -G Ninja)
cmake --build --preset default -j || cmake --build build -j
