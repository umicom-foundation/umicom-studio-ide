# Getting started

Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT

This guide takes you from a fresh clone to a running Umicom Studio IDE.

## Prerequisites
- **C toolchain** (gcc/clang), **Meson** and **Ninja**
- **GTK 4**, **GLib**, **json-glib**
- **ripgrep** (optional, for code search panel)

## Steps
1. Clone the repository and enter the directory.
2. Install dependencies (see platform‑specific how‑tos).
3. Configure and build:

```bash
meson setup build -Dbuildtype=debug
meson compile -C build
./build/umicom-studio
```

If you hit library path issues, check the platform how‑to and TROUBLESHOOTING page.
