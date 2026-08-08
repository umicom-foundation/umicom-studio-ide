# Umicom Studio IDE 0.11.0 validation report

## Source audit basis

- Uploaded local archive: `umicom-studio-ide(2).zip`
- Studio remote `main` inspected at: `e5ffe703a84a5167ca3031b177debffcf6d5d15d`
- Framework remote `main` inspected at: `51e83de1de6c08afea161eace0cd5262f98c813b`

## Corrections applied

- Removed active compatibility path and target names.
- Flattened `applications/studio/src/core/core` into `src/core`.
- Moved Framework composition root to `src/app/bootstrap.c`.
- Replaced the old source list with `StudioSources.cmake`.
- Built the product implementation as `Umicom::StudioProduct`.
- Linked StudioCore and StudioProduct to `Umicom::Framework`.
- Corrected CMake presets, CI, editor paths, Doxygen configuration and current-state documentation.
- Corrected the incomplete `GOptionEntry` sentinel and removed the unused preferences helper that produced the two known Windows baseline warnings.
- Included clean Framework, AuthorEngine and llama.cpp source snapshots.
- Omitted `.git`, generated builds and compiled artifacts from the offline package.

## Automated validation performed

Environment:

- Linux validation container
- CMake 3.31.6
- Ninja 1.12.1
- Clang 17.0.0
- SQLite development library available
- GTK4 development packages unavailable in the validation container

Commands:

```text
cmake -S . -B build-validation -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTING=ON \
  -DUMICOM_STUDIO_BUILD_GTK=OFF \
  -DUMICOM_ENABLE_STRICT_WARNINGS=ON

cmake --build build-validation --parallel 2
ctest --test-dir build-validation --output-on-failure
```

Result:

```text
framework.foundation          PASS
framework.scaffold            PASS
studio.framework_bootstrap    PASS

100% tests passed, 0 tests failed out of 3
```

Additional smoke programs executed successfully:

```text
umicom-framework-demo
umicom-studio-console
```

## Windows GTK status

The uploaded project history and user-provided screenshots show that the
MSYS2 UCRT64 GTK4 application previously configured, linked and opened on the
user's Windows computer. This regenerated source package was not recompiled as
a Windows GTK binary inside the Linux validation container because GTK4 Windows
SDK packages are not installed there.

The accompanying HTML guide therefore requires a clean Windows UCRT64 build,
CTest run and visible GTK launch before the correction is committed and tagged.
