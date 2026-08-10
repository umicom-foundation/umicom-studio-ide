# Validation Report — Framework 0.5.0 / Studio 0.14.0 Release Candidate

## Validation environment

- Linux validation container
- CMake and Ninja
- Clang strict warnings
- GCC strict warnings
- SQLite adapter enabled

## Framework independent validation

- Clang configure: passed
- Clang build: passed with no observed project warnings
- Clang tests: 17 of 17 passed
- GCC configure: passed
- GCC build: passed with no observed project warnings
- GCC tests: 17 of 17 passed
- Native `umicom version`: Framework 0.5.0 ABI 2
- Native `umicom capabilities`: passed, 49 canonical capabilities reported
- Native `umicom check`: compile-link-run probe passed

## Studio combined validation

- Clang configure and build: passed
- Clang tests: 24 of 24 passed
- GCC configure and build: passed
- GCC tests: 24 of 24 passed
- Studio Console: Studio 0.14.0 / Framework 0.5.0
- Framework capability count: 49
- Studio Doctor: 41 passed / 0 failed
- Studio Platform: compiler probe passed
- Studio Diagnostics: demonstration records retained and reported
- Studio Settings: 9 recognised keys / validation passed

## Generated application proof

- Generated `Umicom Designer` repository: passed
- Files generated: 17
- Directories generated: 13
- Clean configure and build: passed
- Generated repository tests: 18 of 18 passed
- Framework installed SDK package: passed
- Generated repository consuming installed SDK: 1 of 1 product test passed

## Remaining local Windows gate

The validation container does not contain the user's Windows MSYS2 UCRT64 GTK4
SDK.  The release is not ready for a remote push until the Windows GTK4 target
builds, opens visibly, and closes normally on the local workstation.
