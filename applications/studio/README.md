# Umicom Studio IDE application

Umicom Studio IDE 0.14.0 is an independent GTK4 and console application built on
Umicom Framework 0.5.0.

## Source ownership

- `src/app` contains the Studio composition root and thin product adapters over
  reusable Framework services.
- `src/console` contains the headless frontend.
- `src/tools` contains native Doctor, diagnostics, settings, and platform tools.
- `src/gtk` contains the GTK4 executable entry point.
- Product-specific editor, workbench, pane, theme, AI, and IDE workflows remain
  inside Studio.
- Reusable filesystem, process, toolchain, repository, scaffolding, capability,
  policy, and suite mechanisms are owned by Framework.

## Platform integration

`UmiStudioPlatformReport` exposes Framework discovery and capability information
to Studio without duplicating the implementation.  The GTK4 environment page
and future repository wizard must use the same Framework services as the native
`umicom` command.

## Application composition

Studio remains independently runnable while sharing the same Framework contracts
that can compose Studio, Designer, Trader, TMS, and future applications into a
suite.
