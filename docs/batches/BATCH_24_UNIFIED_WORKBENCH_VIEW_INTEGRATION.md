# Batch 24 — Unified Workbench View Integration

Created by: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT

Batch 24 turns the Batch 23 Activity Bar from navigation over placeholder pane
labels into navigation over registered Framework view models.

Studio registers nine product view factories:

- Explorer
- Search
- Source Control
- Run and Debug
- Testing
- Visual Designer
- Applications
- Umicom Framework
- AI / AuthorEngine

The factories expose product data as `UmiUiViewModel` properties. They create no
GTK widgets. This preserves the Framework-first boundary and lets GTK4, headless
and future Qt/Wt adapters render the same view data.

The existing Studio services remain authoritative. The views read existing file
index, source-control, build, debugger, testing, designer and AI service state;
they do not duplicate those services.

Batch 24 also completes the Batch 23 CMake integration that was missed when the
old package placed merge material outside the accepted `umicom-studio` tree.
