
# PR Bundle — Wire Chat Pane + Bottom Log (GTK4, Meson)

This bundle provides **minimal code** and **copy/paste snippets** to:
1) Add a **Chat pane** (right side) with a toggle action (View ▸ Chat).
2) Add a **Bottom Log pane** bound to GLib logs.

It does **not** overwrite your files. Instead, follow the steps below and copy snippets into your repo.

---
## 1) Add sources to your tree
Copy these into your repo at the indicated paths:
- `src/panes/chat_pane.h` and `src/panes/chat_pane.c` (from the previous Chat Pane package)
- `src/util/logging.h` and `src/util/logging.c` (from this bundle)
- `src/ui/window_chat_integration.c` (from this bundle)
- `data/ui/chat.ui` (from the previous Chat Pane package)
- If you don't have a bottom log view yet: add `data/ui/partials/log_pane_snippet.xml` to your `main.ui` (see §3).

## 2) Meson additions
Add the following to your `meson.build`:

```meson
# --- Chat + JSON/HTTP deps ---
soup_dep = dependency('libsoup-3.0')
json_dep = dependency('json-glib-1.0')

# --- Sources ---
srcs += [
  'src/panes/chat_pane.c',
  'src/util/logging.c',
  'src/ui/window_chat_integration.c',
]

deps += [ soup_dep, json_dep ]
```

If you use arrays like `srcs = [...]` and `deps = [...]`, append to those. Otherwise, add directly in your `executable()` call.

## 3) main.ui edits (UI injection)
Ensure your main window contains:
- a **right container** with id **`right_box`** (where the Chat pane goes)
- a **bottom container** with id **`bottom_log_scroller`** and a **GtkTextView** with id **`log_view`**

If missing, copy the relevant fragment(s) from `data/ui/partials/`:
- `chat_pane_snippet.xml` — place inside `right_box`
- `log_pane_snippet.xml` — place as a bottom area; if you use a GtkPaned, place this in the lower child

## 4) Resources
Add to `data/ustudio.gresource.xml` inside your `<gresource prefix="/com/umicom/studio">` block:
```xml
<file>ui/chat.ui</file>
```

## 5) Wire-up in code
In your window setup code (often `src/window.c`), after building from `main.ui`, call:

```c
#include "util/logging.h"
#include "ui/window_chat_integration.h"

// Assuming you retrieved the builder or widgets already
GtkTextView *log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "log_view"));
if (log_view) {
    ustudio_log_bind(log_view);           // route GLib logs to the bottom pane
}

// Find right_box container and mount chat pane; also install "app.toggle-chat" action
GtkWidget *right_box = GTK_WIDGET(gtk_builder_get_object(builder, "right_box"));
ustudio_chat_mount_and_actions(app, GTK_WINDOW(window), right_box);
```

## 6) Menu and shortcuts
Add a toggle menu item under **View** (action: `app.toggle-chat`) and an accelerator like `<Ctrl><Shift>C`.
If your UI uses a menu model, insert an item pointing to `"app.toggle-chat"`.

## 7) Build & run
```bash
meson setup build
meson compile -C build
./build/ustudio
```

## Notes
- If you used the **Phase‑1 Sidecar**, set it running on port 8750. The Chat pane will POST to `/chat`.
- The logging helper captures `g_message`, `g_warning`, etc., and appends to the bottom log view.
