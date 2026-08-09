# Umicom Framework and Studio feature plan — 0.13.0

## Implemented in this release

- Framework 0.4.4 typed settings repository.
- String, boolean, integer and real values.
- Schema-owned defaults and descriptions.
- Optional integer and real ranges.
- Reset-one and reset-all operations.
- Human-readable load and save format.
- Studio-owned application schema.
- Native `umicom-studio-settings` command.
- Settings-owned diagnostic-store capacity.
- Canonical Windows UCRT64 headless preset.
- PowerShell environment helper and tool verification.

## Next vertical slices

1. Filesystem path contracts and file watching.
2. Process, build and task execution.
3. Workspace and document models.
4. Problems, commands, queries and events.
5. Git provider gateway.
6. AI provider gateway.
7. Plug-in lifecycle and permissions.
8. Final GTK4 workbench boundaries.

## Architecture rule

Framework owns reusable typed settings mechanics. Studio owns the Studio schema,
defaults and product-specific keys. GTK4 panels may edit settings through the
Studio contract but must not parse settings files directly.
