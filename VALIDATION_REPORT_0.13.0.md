# Umicom Framework 0.4.4 and Studio 0.13.0 validation report

## Feature slice

- Schema-driven typed settings in Framework.
- Studio-owned settings schema and native settings command.
- Settings-controlled diagnostic-store capacity.
- Deterministic Windows MSYS2 UCRT64 headless and GTK presets.

## Clang validation

A clean headless build used Clang 17, CMake and Ninja. All 73 build operations
completed without compiler warnings.

## GCC validation

A second clean headless build used GCC 14.2.0. All build operations completed
without compiler warnings.

## Automated tests

```text
framework.foundation          PASS
framework.scaffold            PASS
framework.diagnostic_hub      PASS
framework.clock               PASS
framework.diagnostic_store    PASS
framework.settings            PASS
studio.framework_bootstrap    PASS
studio.version                PASS
studio.services               PASS
studio.doctor                 PASS
studio.diagnostics            PASS
studio.settings               PASS

100% tests passed, 0 tests failed out of 12
```

## Runtime checks

Studio Console reported Studio 0.13.0, Framework 0.4.4, nine typed settings,
the system theme default, retained diagnostics and one registered module.

Studio Doctor reported 40 passed checks and zero failed checks.

The settings command validated nine recognised keys and listed their types,
values and descriptions.

## Windows status

The Windows presets use absolute UCRT64 compiler, Ninja and pkg-config paths and
prepend the UCRT64 binary directory through the preset environment. The final
Windows headless build, GTK4 build and visible launch remain local release gates.
