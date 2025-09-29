
# Chat Pane – Drop‑in instructions (GTK4, Meson)

**What you’re adding**
- `src/panes/chat_pane.[ch]` – a simple GTK widget that posts to a local sidecar at `http://127.0.0.1:8750/chat`
- `data/ui/chat.ui` – the UI for the pane
- Requires: `libsoup-3.0` and `json-glib-1.0`

---
## 1) Windows: install extra deps in MSYS2 (MINGW64)
```powershell
.\scripts\win\install-chat-deps.ps1
```
This runs (inside MINGW64):
```bash
pacman -S --needed --noconfirm mingw-w64-x86_64-libsoup3 mingw-w64-x86_64-json-glib
```

## 2) Add files
Copy these into your repo:
- `src/panes/chat_pane.h`
- `src/panes/chat_pane.c`
- `data/ui/chat.ui`

## 3) Register UI file in gresource
Edit `data/ustudio.gresource.xml`, add the line below inside the `<gresource prefix="/com/umicom/studio">` block:
```xml
<file>ui/chat.ui</file>
```

## 4) Meson additions
Edit `meson.build`:
- Add the dependencies (top or near the others):
  ```meson
  soup_dep = dependency('libsoup-3.0')
  json_dep = dependency('json-glib-1.0')
  ```
- Ensure the executable sources include `src/panes/chat_pane.c`
- Ensure `dependencies:` for the executable includes `soup_dep` and `json_dep`

Example (illustrative):
```meson
exe = executable('ustudio',
  [
    'src/main.c', 'src/app.c', 'src/window.c',
    'src/panes/chat_pane.c',
  ],
  dependencies: [ gtk_dep, /* ... */ soup_dep, json_dep ],
  install: true
)
```

## 5) Use the widget
In the file that builds your main window (e.g., `src/window.c`), add:
```c
#include "panes/chat_pane.h"

// Once you have a container (e.g., a right-side box) to hold the pane:
GtkWidget *chat = chat_pane_new();
// Optional custom endpoint:
// chat_pane_set_endpoint(chat, "http://127.0.0.1:8750/chat");
gtk_box_append(GTK_BOX(your_right_box), chat);
```

## 6) Build & run
```powershell
# (re)configure + build
meson setup build
meson compile -C build
# run
./build/ustudio.exe
```

## 7) Sidecar (Phase‑1)
Start your local chat sidecar (OpenAI or Ollama). If you used the earlier Windows pack:
```powershell
.\scripts\ai-chat-server.ps1 -Backend openai -Port 8750
# or
.\scripts\ai-chat-server.ps1 -Backend ollama -Port 8750
```
Now the Chat pane “Send” posts to `http://127.0.0.1:8750/chat` and displays the reply.
