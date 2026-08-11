# Application Hub feature origins

This file records only implementation inspiration so future contributors know
why the first Application Hub UI uses these patterns.

- VS Code: workbench/view-container pattern, searchable command-driven UI and
  persistent utility views.
- Geany: compact IDE sidebar and tree-oriented navigation.
- GTK4: `GtkSearchEntry`, list-based views, `GtkPaned`, header bars and model/
  view separation.
- GNOME Builder: IDE workbench concept and tool-oriented project/application
  surfaces.

No source code is copied from those projects. The Batch 19 implementation is a
native Umicom C23/GTK4 design over Umicom Framework public integration contracts.
