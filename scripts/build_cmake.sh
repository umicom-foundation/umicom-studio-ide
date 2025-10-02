#!/usr/bin/env bash
set -euo pipefail
cmake --preset default || (cmake -S . -B build -G Ninja)
cmake --build --preset default -j || cmake --build build -j
