# Umicom Studio IDE – Windows MinGW/Clang build fixes

This zip contains PowerShell scripts + code snippets to fix the exact issues shown in your logs.

**Fixes included**
1) Add GLib include/linking for `test_llm_body` via CMake addendum.
2) Remove duplicate `split_argv_safe` in `src/build/build_system.c`.
3) Fix nested comment in `src/llm/providers/llm_zai.c`.
4) Replace deprecated GTK loop (`gtk_events_pending` / `gtk_main_iteration`) in `src/gui/llm_lab.c` with GLib main-context loop.
5) Add missing includes in `src/gui/app.c` so `UmiApp` and editor APIs resolve.
6) Replace broken SSE parsing block in `src/llm/llm_core.c` with a correct implementation.

## How to use
1. Unzip to your repo root (e.g. `C:\Dev\umicom-studio-ide\`).
2. Run:
   ```powershell
   .\scripts\apply-umicom-fixes.ps1
   .\scripts\verify-build.ps1
   ```
If any patch cannot be applied automatically, the script will tell you and point to the snippet in `patches\*` to paste manually.
