# Umicom Studio IDE application

This directory contains the Framework-based Umicom Studio composition root.

## Current migration stage

- `src/core` contains new product-neutral Studio bootstrap code.
- `src/console` provides a headless lifecycle check.
- `src/gtk` starts the existing GTK application through Umicom Framework.
- `src/legacy` contains the preserved pre-Framework Studio implementation.
- root-level `assets`, `config`, `win`, `scripts` and legacy `tests` are kept in place temporarily to preserve behaviour.

The next vertical slice migrates diagnostics and logging from the compatibility source into Umicom Framework.
