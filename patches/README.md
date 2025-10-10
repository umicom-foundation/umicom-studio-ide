# Umicom Studio IDE – minimal fixes to get you building/running on MSYS2

This package includes:
- `CMakeLists.txt` – a **drop-in** replacement.
- `PATCHES/*.patch` – small, focused patches for the issues in your logs.

## How to apply

1) Back up your repo (or commit your current work).

2) Copy this `CMakeLists.txt` to the root of your repo (replacing the old one).

3) Apply the patches (they are tiny; if a hunk fails, do the edit by hand):

```
git apply PATCHES/0001-fix-app-c-weak-prototypes.patch || echo "Edit src/gui/app.c manually: change the weak declaration to return gboolean"
git apply PATCHES/0002-llm-lab-include.patch          || echo "Edit src/gui/llm_lab.c manually: change include to #include \"llm_lab.h\""
git apply PATCHES/0003-run_config-fix-misleading-indentation.patch || echo "Edit src/run_config.c manually: split the single-line if into braces and individual g_free lines"
git apply PATCHES/0004-fix-warnings-unused-params.patch || echo "If any hunk fails, apply the one-liners by hand"
```

4) Configure + build (console mode ON so you can see logs):

```
cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSIDE_DEV_CONSOLE=ON
ninja -C build-ninja -j1
```

5) Run:

```
$Env:G_MESSAGES_DEBUG = "all"
$Env:G_DEBUG          = "fatal-warnings"
$Env:Path             = "C:\msys64\mingw64\bin;$Env:Path"

.\build-ninja\ustudio.exe --help
```

## Why these changes?

- **Undefined reference to `WinMain`**: happens when a GUI subsystem is linked
  without a `WinMain`/`wWinMain`. We compile `src/win_launcher.c` which *provides*
  `wWinMain/wmain` and bridges to your `main()`, so both console and GUI builds
  work properly.

- **`umi_editor_save` conflicting types**: a weak proto in `src/gui/app.c` used
  `void` but the public header declares `gboolean`. GCC rejects this. Patch 0001
  makes the weak declaration match the header exactly.

- **`umicom/llm_lab.h` not found**: the header lives under `src/llm/`, and the
  include dirs are already added. Use `#include "llm_lab.h"` (Patch 0002).

- **Warnings**: the remaining warnings in your logs are mostly unused params,
  a misleading indentation, and deprecated GTK 4 APIs used on purpose (TreeView,
  Dialog). Patch 0004 removes the unused bits and wraps deprecated calls in
  `G_GNUC_BEGIN/END_IGNORE_DEPRECATIONS` so the build is clean while you decide
  whether/when to port to GtkListView etc.
