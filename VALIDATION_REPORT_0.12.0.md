# Umicom Framework 0.4.3 and Studio 0.12.0 validation report

## Source baseline

- Studio baseline: 0.11.1 at `cdcd69be92a61a68562fe32dde9d440486249bf5`.
- Framework baseline: 0.4.2 at `9e4e1351d7790fec02d45dec83fe9b22684b9c32`.
- Feature slice: bounded retained diagnostics and native diagnostics command.

## Clang validation

The combined source was configured from an empty headless build directory with
Clang 17, CMake and Ninja. All 65 build operations completed without compiler
warnings.

## GCC validation

A second combined headless build used GCC 14.2.0. The build completed without
compiler warnings.

## Automated tests

```text
framework.foundation          PASS
framework.scaffold            PASS
framework.diagnostic_hub      PASS
framework.clock               PASS
framework.diagnostic_store    PASS
studio.framework_bootstrap    PASS
studio.version                PASS
studio.services               PASS
studio.doctor                 PASS
studio.diagnostics            PASS

100% tests passed, 0 tests failed out of 10
```

## Runtime smoke tests

Studio Console reported Studio 0.12.0, Framework 0.4.3, one registered module,
and retained diagnostic counters.

Studio Doctor reported:

```text
Checks passed: 32
Checks failed: 0
```

The diagnostics command retained five records during the demonstration run:
three Framework lifecycle information records, one warning and one error.

## Windows GTK4 status

The source preserves the GTK4 product application. The final Windows MSYS2
UCRT64 build, CTest run and visible GTK4 launch must be completed on the Windows
development machine before the Studio 0.12.0 release tag is created.
