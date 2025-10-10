USIDE Fix Pack – Apply Instructions
===================================

1) BACKUP your working tree (zip it or commit everything).

2) Copy these files over your repository root, preserving the folders:
   - CMakeLists.txt
   - src/win_launcher.c
   - src/build/build_queue.c
   - src/crash_guard.c
   - src/msys_env.c
   - src/output_pipeline.c
   - src/run_config.c
   - src/search/search_panel.c
   - src/theme.c
   - src/gtk_smoke.c  (optional dev-only)

3) IMPORTANT: Fix src/gui/app.c (conflicting types error)
   Open 'src/gui/app.c' and remove any weak forward declarations like:
       __attribute__((weak)) void umi_editor_save(...);
       __attribute__((weak)) void umi_editor_save_as(...);
   Then add the correct header include near the top:
       #include "editor/editor.h"
   The real prototypes there return 'gboolean', matching the definitions.

4) Configure and build (Console mode ON to see logs):
   PowerShell:
     Remove-Item -Recurse -Force .\build-ninja -ErrorAction SilentlyContinue
     cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSIDE_DEV_CONSOLE=ON
     ninja -C build-ninja -j1

5) Run:
   $env:G_MESSAGES_DEBUG = "all"
   $env:G_DEBUG = "fatal-warnings"
   $env:Path = "C:\msys64\mingw64\bin;$env:Path"
   .\build-ninja\ustudio.exe --help

6) For a GUI (no console) release build on Windows:
   cmake -S . -B build-release -G Ninja -DUSIDE_DEV_CONSOLE=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo
   ninja -C build-release
