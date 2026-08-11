# Feature origins — engineering note only

Batch 20 uses proven IDE/editor interaction patterns as inspiration:

- GNOME Builder: fuzzy file/symbol search, side-by-side editing and IDE workbench.
- Geany: lightweight document list, recent-project/session behaviour.
- GNOME Text Editor: incremental find/replace and editor-focused simplicity.
- VS Code: Quick Open, open editors, navigation history and status-bar patterns.
- GtkSourceView 5: syntax highlighting, source buffers, line numbers, completion,
  snippets and search/replace primitives.

No source code is copied from these projects. The implementation is native
Umicom C23/GTK4 code and keeps reusable/product boundaries explicit.
