# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------

New-Item -ItemType Directory -Force -Path build | Out-Null
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
