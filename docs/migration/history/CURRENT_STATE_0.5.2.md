# Umicom Studio IDE migration current state

## Verified baseline

- Baseline tag: `studio-pre-framework-v0.9.0`
- Original GTK4 application: builds and launches on Windows
- Compiler: MSYS2 UCRT64 Clang
- Build system: CMake and Ninja

## Current branch

`migration/framework-0.5`

## Completed migration actions

- Added `umicom-framework` as a Git submodule.
- Created `applications/studio` migration directories.
- Moved the original `src` modules under `applications/studio/src/legacy`.
- Moved the original `src/main.c` to `applications/studio/src/gtk/main.c`.

## Stage 0.5.2 work

- Complete Framework Foundation 0.4.1 in the Framework repository.
- Update the Studio submodule pointer.
- Add the Framework-based Studio bootstrap.
- Replace the monolithic root CMake build with explicit targets.
- Build the headless lifecycle test.
- Rebuild and launch the preserved GTK application.

## Intentionally not moved yet

- `assets`
- `config`
- `win`
- `scripts`
- existing root `tests`
- `third_party`

These stay in their original locations until their dependencies and runtime paths are migrated safely.


## Archive audit checkpoint

- `applications/studio` already exists.
- `applications/studio/CMakeLists.txt` and the Framework bootstrap files were missing in the uploaded checkpoint.
- Root CMake and CMake presets were already migrated.
- Visual Studio Code tasks were still the old Meson tasks and must be replaced.
