# Build and Validate Framework 0.5.0 and Studio 0.14.0

## Development policy

This is a local major-release candidate.  Do not push or tag either repository
until the complete validation list is satisfied.

## Windows native workflow

After the native tool has been built once, normal commands are:

```text
umicom check --gtk --github
umicom make --source . --build build/windows-ucrt64-headless-debug
umicom run --source . --build build/windows-ucrt64-debug --target umicom-studio-ide
```

The native command creates the child environment required by CMake, Ninja,
pkg-config, windres, GTK4, and runtime DLL discovery.  The PowerShell UCRT64
script remains only a fallback for the first bootstrap build.

## Required validation

- Framework Clang strict build.
- Framework GCC strict build.
- All Framework tests.
- Native environment compile-link-run probe.
- Native GTK package probe on Windows.
- Native repository dry-run.
- Complete generated Umicom Designer repository build and tests.
- Studio headless build and all tests.
- Studio Console, Doctor, diagnostics, settings, and platform commands.
- Studio GTK4 visible launch and normal shutdown.
- Doxygen generation.
- No unplanned changed files or comment-only changes.
- Exact `File:` path audit for every changed C and header file.
