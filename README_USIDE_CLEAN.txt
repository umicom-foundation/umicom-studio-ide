Umicom Studio IDE — Clean Build Drop
=======================================

What changed
------------
1) **CMakeLists.txt** is fully rewritten and commented:
   - Fixes the earlier `-D-IC:/...` bug (that came from treating pkg-config CFLAGS
     as **preprocessor definitions**). We now pass include directories to
     `target_include_directories()` and only pass the `*_CFLAGS_OTHER` as options.
   - Adds **console vs GUI** switch through `-DUSIDE_DEV_CONSOLE=ON|OFF`.
   - Keeps MinGW `-municode` glue in compile & link options (matches your old build).
   - Uses `GLOB_RECURSE` while the tree is still moving quickly.

2) **src/umicom/llm_lab.h** is a tiny *bridge header* so this include works:
   ```c
   #include "umicom/llm_lab.h"
   ```
   It forwards to `src/gui/llm_lab.h`. Drop it in and you won't need to retouch
   existing source files that include the "umicom/" path.

One **manual 1‑liner** you must apply
-------------------------------------
The current tree still declares a weak symbol in `src/gui/app.c` with a wrong
return type, which causes this compile error:

```
error: conflicting types for 'umi_editor_save'
note: previous declaration with type 'gboolean(UmiEditor*, GError**)'
```

**Fix the declaration in `src/gui/app.c`:**

```c
// OLD (wrong)
__attribute__((weak)) void     umi_editor_save(UmiEditor *ed, GError **error);

// NEW (correct)
__attribute__((weak)) gboolean umi_editor_save(UmiEditor *ed, GError **error);
```

That makes the weak prototype match `src/editor/editor.h` and the link will
succeed when the real implementation is present (or you can feature‑gate calls).

Build commands
--------------
**Console (recommended while debugging):**
```
cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSIDE_DEV_CONSOLE=ON
ninja -C build-ninja -j1
.\build-ninja\ustudio.exe --help
```

**GUI (no console window):**
```
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUSIDE_DEV_CONSOLE=OFF
ninja -C build-release -j1
```

Why you saw “macro names must be identifiers”
---------------------------------------------
That came from command lines like `-D-IC:/msys64/mingw64/include ...`. The `-D`
prefix was injected because compile definitions were filled with raw `-I...`
tokens (pkg-config CFLAGS). The new CMake avoids that by sending:
- include paths to `target_include_directories()`
- non-include flags to `target_compile_options()`

Cleanup plan (short)
--------------------
- Keep the tree compiling first; then migrate deprecated GTK widgets later.
- If a file is truly experimental (e.g. `src/gtk_smoke.c`), move it under
  `src/experiments/` so it’s not built.
- If `src/palette.c` is legacy (you already have `src/command_palette.c`),
  remove it or move it out of the default build.

See `CLEANUP_PLAN.txt` for the suggested moves/removals.
