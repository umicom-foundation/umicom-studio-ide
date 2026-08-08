# Umicom Studio IDE Current State 0.6.0

## Completed

- Preserved and tagged the pre-Framework 0.9.0 baseline.
- Added Umicom Framework as a pinned submodule.
- Added a Framework Master Controller around Studio lifecycle.
- Added headless Framework and Studio tests.
- Replaced obsolete Meson and Android CI checks with CMake/Ninja headless CI.
- Promoted the existing Studio implementation into the active product source
  tree under `applications/studio/src`.
- Replaced `Umicom::StudioLegacy` with `Umicom::StudioProduct`.

## Verified commands

```powershell
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug

cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug
ctest --preset windows-ucrt64-debug
```

## Current architecture

- `Umicom::Framework` provides shared runtime capabilities.
- `Umicom::StudioCore` owns the composition root.
- `Umicom::StudioProduct` contains the current product implementation.
- Console and GTK4 are separate frontend executables.

## Remaining migration work

1. Diagnostics and logging.
2. Time, UTF-8, arguments and environment.
3. Filesystem, paths and file watching.
4. Configuration, settings and sessions.
5. Process, build and task execution.
6. Problems and diagnostic routing.
7. Workspace and document models.
8. Command, query and event routing.
9. Git provider contract.
10. AI Provider Gateway.
11. Plug-in loading and permissions.
12. Editor and pane presentation boundaries.
