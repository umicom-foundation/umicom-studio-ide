# Umicom Studio IDE application

Umicom Studio IDE 0.13.0 is an independent application built on Umicom
Framework 0.4.4 or later.

## Source layout

- `src/app` contains the Framework composition root, shared services,
  diagnostics adapter and Studio settings schema.
- `src/console` contains the console frontend.
- `src/tools` contains native Doctor, diagnostics and settings commands.
- `src/gtk` contains the GTK4 executable entry point.
- `src/build`, `src/core`, `src/editor`, `src/gui`, `src/llm`, `src/panes`,
  `src/plugins`, `src/search`, `src/ui` and `src/util` contain the active Studio
  product implementation.
- `include/umicom/studio` contains public Studio contracts.
- `tests` contains Framework integration and feature tests.

## Settings ownership

Framework owns the reusable typed repository. Studio owns the product schema,
defaults and ranges. GTK4 code must use the Studio settings contract rather
than parse `config/studio.settings` directly.

The product source is compiled as `Umicom::StudioProduct`. Reusable capabilities
continue to move into Umicom Framework one tested subsystem at a time.
