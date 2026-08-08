# Umicom Studio IDE 0.11.1 validation report

## Source audited

- Studio remote base: `27688b6bea268262d2eef43ef232d0312751ec07`.
- Framework remote base: `51e83de1de6c08afea161eace0cd5262f98c813b`.
- AuthorEngine: `fc1ca481022b60eb6f6b52d7e95256e85192e09e`.
- llama.cpp: `a4a0aa5ea2a88e4858996199fefd5439f17b481c`.

## Clean-copy build

The source was copied to a new directory without Git metadata or generated
build output, configured with Clang 17, built with CMake and Ninja, and tested
through CTest.

```text
framework.foundation          PASS
framework.scaffold            PASS
framework.diagnostic_hub      PASS
framework.clock               PASS
studio.framework_bootstrap    PASS
studio.version                PASS
studio.services               PASS
studio.doctor                 PASS

100% tests passed, 0 tests failed out of 8
```

The Framework demonstration, Studio console and native Studio doctor also ran
successfully. The doctor reported 24 passed checks and zero failed checks.

A second headless build was completed with GCC 14.2.0. All eight tests passed,
and the updated source compiled without warnings in that validation run.

## Windows GTK4 status

The supplied user output proves that Studio 0.11.0 built and opened under the
Windows MSYS2 UCRT64 environment. Studio 0.11.1 retains the product GTK source,
but its Windows GTK build must be run on the user's Windows machine before the
0.11.1 release tag is created.

## Documentation workflow

The 0.11.1 workflow creates `build/doxygen` before invoking Doxygen, correcting
the failure observed on Studio 0.11.0. The container used for this package did
not have a working Doxygen installation, so GitHub Actions remains the final
documentation validation gate.

## Architectural status

Active `src/legacy`, `StudioLegacy`, `LegacySources.cmake` and
`UMICOM_STUDIO_BUILD_LEGACY_GTK` build references are absent. Studio now has a
Framework-owned diagnostic fan-out service, an explicit Studio service
container, native version API and native doctor. Other duplicated product
services remain scheduled for bounded extraction; see
`docs/migration/FRAMEWORK_DEPENDENCY_MATRIX_0.11.1.md`.
