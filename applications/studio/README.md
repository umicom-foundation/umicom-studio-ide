# Umicom Studio IDE application

Umicom Studio IDE is an independent application built on Umicom Framework.

## Source layout

- `src/app` contains the Framework composition root.
- `src/console` contains the console frontend.
- `src/gtk` contains the GTK4 executable entry point.
- `src/build`, `src/core`, `src/editor`, `src/gui`, `src/llm`, `src/panes`,
  `src/plugins`, `src/search`, `src/ui` and `src/util` contain the active Studio
  product implementation.
- `include/umicom/studio` contains public Studio contracts.
- `tests` contains Framework integration tests.

The product source is no longer compiled through a compatibility or legacy
library.  It is compiled as `Umicom::StudioProduct`.  Reusable capabilities
will continue to move into Umicom Framework one tested subsystem at a time.
