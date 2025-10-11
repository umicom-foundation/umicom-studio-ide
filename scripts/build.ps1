# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Project automation / helper script.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------
# Build on Windows (PowerShell)
Param([switch]$Run,[switch]$Test)
meson setup build -Dbuildtype=debug || meson setup build --reconfigure
meson compile -C build
if ($Test) { meson test -C build }
if ($Run)  { ./build/umicom-studio }
