# Architecture overview

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

High‑level modules:

- **Editor**: text, problem markers, navigation hooks.
- **Output**: ANSI console, filters, pipeline.
- **Search**: ripgrep runner and UI panel.
- **Build**: tool detection, runner, and output routing.
- **Settings**: JSON store + settings bus for live updates.
- **Integrations**: Windows/MSYS helpers, options parser, logging.
- **UX**: command palette, preferences dialog.

Design goals: simple APIs, heavy comments, and minimal coupling between subsystems.
