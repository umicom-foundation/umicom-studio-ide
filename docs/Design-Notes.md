
# Umicom Studio — Design Notes (Draft)

## Screen map (ASCII)
```
+------------------------------------------------------------+
| HeaderBar: [Open]            Umicom Studio         [Menu]  |
+-----------------+------------------------------------------+
| Project         |  [ Welcome Tab ] [ file.c ] [ book.uai ] |
| - repo root     |  +------------------------------------+  |
| - src/          |  |  Editor area / views               |  |
| - docs/         |  |  (TextView now; code editor later) |  |
| - assets/       |  +------------------------------------+  |
+-----------------+------------------------------------------+
| Logs: build/run output, diagnostics                         |
+------------------------------------------------------------+
Status: line/col, mode, branch
```

## Panels
- **Project** (left): file tree & context actions
- **Editor tabs** (center): code/text editors, preview panes
- **Logs** (bottom): tasks, diagnostics, LSP output

## Next steps
- Hook up Gio::File/GLib file enumeration to populate `project_list`
- Add LSP client for UAI and common languages
- Implement command palette and action accelerators
