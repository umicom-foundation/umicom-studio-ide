# umicom-studio-ide

UStudio — IDE/editor &amp; plugin host


## Umicom Studio IDE — GTK4 Skeleton

A minimal-but-real GTK4 skeleton for **umicom-studio-ide** with Meson as the primary build system.
It includes:

- `GtkApplication` entry, an application window with a HeaderBar
- Left **Project** sidebar (placeholder list), main **Editor Notebook**, bottom **Log/Terminal** pane
- Keyboard shortcuts window, CSS styling, and GResource bundling
- **Mockups** (SVG wireframes) and a design notes doc
- Optional **CMake** build file
- VS Code tasks/launch and idempotent bootstrap scripts

> This skeleton is safe to drop into an empty repo. If files already exist, prefer your existing ones.
> Scripts are **idempotent** and will only create missing files/folders.

## Build (Meson — Linux/macOS)
```bash
# GTK4 + toolchain
# Debian/Ubuntu: sudo apt install build-essential meson ninja-build pkg-config libgtk-4-dev
# Fedora:        sudo dnf install gcc gcc-c++ meson ninja-build pkgconf-pkg-config gtk4-devel
# Arch:          sudo pacman -S base-devel meson ninja pkgconf gtk4
meson setup build
meson compile -C build
./build/ustudio
```

## Build (Meson — Windows via MSYS2)
```bash
# In "MSYS2 MINGW64" shell:
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-gtk4 mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-pkgconf
meson setup build
meson compile -C build
./build/ustudio.exe
```

## Optional: Build (CMake)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
./build/ustudio
```

## Project layout
```
.
├─ meson.build
├─ CMakeLists.txt            (optional)
├─ src/
│  ├─ main.c
│  ├─ app.h
│  ├─ app.c
│  ├─ window.h
│  └─ window.c
├─ data/
│  ├─ ui/
│  │  ├─ main.ui
│  │  └─ shortcuts.ui
│  ├─ styles/app.css
│  └─ ustudio.gresource.xml
├─ assets/mockups/
│  ├─ wireframe-dashboard.svg
│  └─ wireframe-editor.svg
├─ docs/Design-Notes.md
├─ scripts/
│  ├─ bootstrap-windows.ps1
│  ├─ bootstrap-posix.sh
│  ├─ build-meson.ps1
│  └─ build-meson.sh
└─ .vscode/
   ├─ tasks.json
   └─ launch.json
```

---
## First run (quick)
```bash
meson setup build && meson compile -C build && ./build/ustudio
```
