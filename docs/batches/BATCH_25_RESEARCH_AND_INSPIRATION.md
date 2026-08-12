# Batch 25 — Research and Architectural Inspiration

Created by: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT

## Purpose

Batch 25 uses mature open-source application platforms as architectural inspiration while keeping Umicom's implementation original, C23-first, toolkit-neutral at the Framework boundary, and compatible with the existing Umicom Master/Slave Controller, Publisher/Subscriber, command/event/query, Data Server and Integration Fabric architecture.

No third-party source code is copied into this batch. The projects below are used to study interaction models, extension boundaries, application composition and developer ergonomics.

## Projects studied

- Wt — https://github.com/emweb/wt and https://www.webtoolkit.eu/wt/
- Glade — https://glade.gnome.org/ and https://gitlab.gnome.org/Archive/glade
- GTK — https://gitlab.gnome.org/GNOME/gtk/
- Geany — https://github.com/geany/geany
- GNOME Software — https://gitlab.gnome.org/GNOME/gnome-software
- GIMP — https://gitlab.gnome.org/GNOME/gimp
- GNOME Builder — https://gitlab.gnome.org/GNOME/gnome-builder
- Geary — https://gitlab.gnome.org/GNOME/geary
- GNOME Text Editor — https://gitlab.gnome.org/GNOME/gnome-text-editor
- GNOME Shell — https://gitlab.gnome.org/GNOME/gnome-shell
- Nautilus / Files — https://gitlab.gnome.org/GNOME/nautilus
- Visual Studio Code — https://github.com/microsoft/vscode
- TradingView open-source projects — https://github.com/tradingview

## Patterns adopted as Umicom concepts

1. **Model-driven UI rather than toolkit-owned business state.** Batch 25 adds list, tree, selection, sorting/filtering, docking, tabs, panels, context menus, property inspection, command surfaces and undo/redo models to Framework.
2. **Extension and contribution boundaries.** New extension-point and command-surface contracts complement the existing Framework contribution, plug-in and command registries.
3. **Persistent workbench composition.** Dock, panel and tab models are designed so Studio and future applications can persist their own layouts without putting product state into GTK.
4. **Server-driven multi-frontend composition.** Widget trees, signals, bindings, sessions, transports, render trees and style rules provide reusable primitives for Wt-like server-side application composition while remaining Umicom-native C contracts.
5. **File-manager-grade resource navigation.** Recent resources, bookmarks, normalised resource locations, workspace history and queued file operations are reusable beyond Studio.
6. **Software-centre/product catalogue.** Marketplace items, metadata providers, installation state and update policies build on the existing Umicom delivery, product, plug-in, security and compatibility foundations.
7. **Financial chart interaction.** Multi-pane layout, independent scales, crosshairs, markers, annotations, drawings, live-stream state and chart extensions build on the existing Framework chart and trading foundations.
8. **Visual-authoring infrastructure.** Property schemas, signal/action bindings, alignment operations, designer clipboard and template palettes extend the existing declarative language and designer engine.
9. **Reusable editor platform.** Document, cursor, selection, marker, folding, symbol, diagnostic, completion, code-action, diff and configuration models provide a common C23 editor foundation that can consume native parsers or Language Server results.
10. **Owned aggregate services.** Workbench Platform, Resource Centre, Frontend Application, Product Centre, Chart Workspace, Designer Authoring Session and Editor Session provide convenient reusable ownership boundaries for future products.

## Boundary rule

GTK4 remains an adapter. Public Framework headers introduced in this batch do not expose GTK object types. Studio composes the generic Framework capabilities into product-specific centres and workspaces; future Umicom Trader, TMS, banking, exchange, media and other applications can reuse the same Framework contracts without inheriting Studio-specific UI code.
