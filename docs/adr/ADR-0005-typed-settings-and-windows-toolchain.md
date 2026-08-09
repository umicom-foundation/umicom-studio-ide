# ADR-0005: Typed settings and deterministic Windows toolchain selection

**Status:** Accepted  
**Release:** Umicom Framework 0.4.4 / Umicom Studio IDE 0.13.0

## Decision

1. Umicom Framework owns a toolkit-neutral `UmiSettings` repository.
2. Applications provide immutable schemas containing keys, types, defaults,
   descriptions and optional numeric ranges.
3. Studio owns its schema in `applications/studio/src/app/settings.c`.
4. Studio's service container owns one settings repository and loads the
   optional `config/studio.settings` file before creating dependent services.
5. The settings file is human-readable `key=value` text and is validated before
   values enter the repository.
6. Windows developers use dedicated UCRT64 presets with absolute compiler,
   Ninja, pkg-config and prefix paths.
7. The portable `headless-debug` preset remains available for Linux and CI.

## Consequences

- Product modules no longer need to parse configuration files independently.
- Invalid booleans, numbers and out-of-range values are rejected consistently.
- New PowerShell sessions cannot silently select standalone LLVM when the
  Windows UCRT64 preset is used.
- Future GTK4 settings panels can bind to one product schema.
