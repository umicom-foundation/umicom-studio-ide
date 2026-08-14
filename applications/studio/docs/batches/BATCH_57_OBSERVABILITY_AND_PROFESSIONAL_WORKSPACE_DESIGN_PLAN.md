# Batch 57 — Observability Centre and Professional Workspace Design Plan

Created by Sammy Hegab, Umicom Foundation. MIT licence.

## Outcome

Batch 57 delivers the planned Observability, Performance and Resilience Centre and establishes a reusable professional-workspace model for Umicom Studio, Trader, TMS and future applications. It converts the requirements in *Umicom Studio Ideas 2* into Framework contracts, Studio presets, commands, tests and a staged UI implementation plan.

The design borrows proven interaction ideas from professional IDEs, comparison tools and trading workstations. It does not copy another product's visual identity. The Umicom model generalises those ideas into accessible, toolkit-neutral contracts.

## Source-driven requirements

| Source idea | Umicom interpretation | Batch 57 foundation |
|---|---|---|
| TWS-style custom layouts | Named bottom-level workspace tabs with independent panel arrangements | `UmiUiWorkspaceLayoutV3`, layout library and Develop/Operations/Compare presets |
| TWS New Window catalogue | Searchable, categorised tool/window picker | 18 Studio window descriptors across development, navigation, operations, data, AI and trading |
| TWS linked panels | Context groups that route selection between windows | Project, run and file groups with source, destination and bidirectional roles |
| TWS lock/edit workflow | Safe default prevents accidental movement; explicit edit mode enables organisation | Layouts are locked by default; add, remove and move require unlock |
| TWS save/recovery | Versioned local persistence plus a future Data Server adapter | `UMILAYOUT2` schema, deterministic encode/decode and import/export boundary |
| Visual Studio/VS Code familiarity | Explorer, tabbed editor, tool windows, output area, command surface | Develop preset and reusable catalogue entries |
| Beyond Compare workflow | Explorer plus original/modified panes and result/output region | Compare preset and file-context group |
| Modern application appearance | Design tokens, system/light/dark/high-contrast modes, density and font scale | `UmiUiThemeProfileV2` and Studio theme commands |
| Operational IDE | Metrics, traces, profiling, benchmarks, crashes, health and resilience in project/run context | Framework `Umicom::observability` and Studio Operations Centre v2 |

## Interaction design

### Workspace chrome

The GTK shell should render one stable application anchor containing the global menu, command search, project/run context, New Window action, layout lock and appearance controls. Layout tabs sit at the bottom of the workspace. Each layout owns its panel tree, selected tabs, linked groups and maximised panel state.

Locked mode is the normal working state. In edit mode, the workspace receives a clearly visible outline and an “Editing layout” status indicator. Dragging shows snap targets for left, right, top, bottom, centre-tab and floating-window placement. Closing, moving or resizing a panel is undoable until the layout is saved.

Double-clicking a panel title maximises it inside the active workspace without changing the saved geometry. Repeating the action restores the previous arrangement. This remains available while the layout is locked.

### New Window catalogue

The catalogue is command-searchable and grouped by role. It should support:

- single-instance and multi-instance tools;
- default size and preferred docking location;
- drag from the catalogue into a snap target;
- keyboard insertion into the current tab stack;
- window aliases and recently used tools;
- application-specific contributions without hard-coding them in the shell.

The initial Studio catalogue includes Project Explorer, Editor, File Compare, Terminal, Build, Test Explorer, Debug, Source Control, Output, Metrics, Traces, Profiler, Health and Resilience, Database Explorer, AI/AuthorEngine, Chart, Watchlist and Order Entry.

### Linked context groups

Colour alone is not sufficient for accessibility, so every group combines a colour token, name, icon and context kind. The Framework supports file, project, symbol, account, run, diagnostic and generic context. Each member declares whether it publishes context, consumes context or does both.

Examples:

- selecting a project in Project Explorer updates Editor, Build and AuthorEngine;
- selecting a run updates Metrics, Traces, Profiler and Health;
- choosing an original file updates Modified and Comparison Results;
- selecting a symbol in Trader can update Chart, Watchlist and Order Entry;
- selecting an account in a TMS application can update Positions, Cash, Risk and Settlements.

Context messages should carry a type, stable identifier, origin window, correlation identifier and monotonic sequence. The future renderer must suppress loops when bidirectional panels relay the same selection.

### Layout persistence and recovery

The Framework serialises only portable layout state. Filesystem and Data Server adapters remain application responsibilities. The production implementation should save through atomic replacement, keep the previous known-good version and retain a short recovery history.

Persisted state includes schema version, layout identity, lock state, panel instances, tool identities, normalised geometry, visibility, floating/maximised state, z-order and group identity. Later schema versions add tab stacks, split ratios, multi-monitor affinity, per-window settings and keyboard focus history.

Secrets, access tokens, database passwords and provider credentials must never be embedded in a layout. A panel stores only a reference to an approved connection profile.

### Appearance system

All surfaces consume semantic tokens rather than literal colours. The first token set covers background, surface, foreground, accent, success, warning and danger. The next renderer layer should add borders, focus, selection, hover, disabled, chart palettes, editor syntax, spacing, corner radius and motion duration.

System, light, dark and high-contrast modes are first-class profiles. Compact, comfortable and spacious density settings are independent from font scaling. The shell must preserve usable hit targets and never rely solely on colour to communicate linked state, severity or selection.

## Supplied presets

### Develop

- Project Explorer on the left;
- tabbed Editor in the centre;
- AI and AuthorEngine on the right;
- Output across the bottom;
- linked project context.

### Operations

- Metrics and Traces on the upper row;
- Profiler and Health/Resilience on the lower row;
- linked run context;
- project and run filters should remain visible when a panel is maximised.

### Compare

- folder/file explorer at the left;
- Original and Modified side by side;
- Comparison Results below the two documents;
- linked file context;
- planned merge actions: accept left, accept right, accept block, next difference and previous difference.

## Architecture

The boundary is deliberately layered:

1. **Framework model:** layout, window catalogue, grouping, layout library, theme profile and persistence contracts in `Umicom::ui`; operational histories, correlation, health and resilience evidence in `Umicom::observability`.
2. **Studio composition:** Studio window contributions, presets, linked groups, workspace commands/views and Operations Centre project/run views.
3. **GTK renderer:** dock/split/tab widgets, drag targets, floating windows, bottom layout tabs, anchor controls and accessibility semantics.
4. **Persistence adapters:** local atomic file first, then Data Server synchronisation, backup and recovery.

No GTK type crosses the Framework or StudioCore public boundary. The same contracts can therefore drive Qt6, Wt, GTK4, web and headless tests.

## Implementation plan

### Batch 57 — delivered foundation

- Framework observability histories, correlation, health summary, resilience telemetry and ten-view catalogue;
- Studio Operations Centre with seeded project/run evidence, views and commands;
- professional layout, window catalogue, linked group, preset library, theme and persistence contracts;
- Develop, Operations and Compare presets;
- native inspection commands and focused regression tests.

### Renderer wave 1 — workspace shell

- add bottom layout tabs and the application anchor to the GTK workbench;
- render the normalised layout model as nested split/tab containers;
- add New Window search, panel title menus and maximised/restored behaviour;
- expose lock/edit state with keyboard commands and accessible status text.

### Renderer wave 2 — design and organisation

- drag-and-snap placement, tab reordering, nested subwindows and floating windows;
- layout create, clone, rename, reorder and delete flows;
- undo/redo for every layout edit;
- multi-monitor bounds validation and off-screen recovery.

### Persistence wave

- atomic local save, Save As, import/export and previous-version recovery;
- Data Server repository for optional synchronisation across machines;
- layout migration registry and schema compatibility tests;
- autosave only after a validated edit transaction.

### Context wave

- typed context bus with loop prevention and event correlation;
- group badge/menu on every panel;
- application contributions for Studio projects/runs, Trader symbols/accounts and TMS entities;
- saved group templates and accessible non-colour identifiers.

### Professional tools wave

- native two-file and directory Compare Centre with merge actions;
- profiler timelines, flame views, benchmark comparisons and run-to-trace navigation;
- build/package/deploy task profiles for C, C++, Zig, Rust, Assembly, CMake and Bash;
- layout/template marketplace after signing, compatibility and policy controls are available.

## Acceptance criteria for the GTK renderer

- A user can create a layout, add at least four panels, move/resize/tab them, lock the layout and restore it after restart.
- A user can switch between Develop, Operations and Compare without losing unsaved document state.
- Selecting a project, run, file or symbol updates only compatible members of the selected group.
- Keyboard-only users can open New Window, place a panel, switch layouts, maximise/restore and lock/unlock.
- Corrupt or unsupported layout data is rejected without replacing the last known-good layout.
- High-contrast mode and 200% font scaling remain navigable without clipped primary actions.
- No raw credential or secret appears in the persisted layout document.
