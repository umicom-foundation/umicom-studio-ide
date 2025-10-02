/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/BUILD_NOTES.c
 * PURPOSE: Build & toolchain instructions (Windows/MSYS2, Linux, macOS).
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/*
Windows (MSYS2 / UCRT64)
------------------------
1) Install MSYS2 (UCRT64) and open the UCRT64 shell.
2) pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
3) pacman -S mingw-w64-ucrt-x86_64-gtk4 mingw-w64-ucrt-x86_64-glib2             mingw-w64-ucrt-x86_64-json-glib
4) Ensure pkg-config finds gtk4/glib/json-glib.
5) Build:
   clang -std=c17 -O2 -Wall -Wextra -Wpedantic \
     $(pkg-config --cflags gtk4 glib-2.0 gio-2.0 json-glib-1.0) \
     -o umicom.exe \
     src/*.c \
     $(pkg-config --libs gtk4 glib-2.0 gio-2.0 json-glib-1.0)

Linux (Debian/Ubuntu)
---------------------
sudo apt-get install -y build-essential clang pkg-config libgtk-4-dev libglib2.0-dev libjson-glib-dev
clang -std=c17 -O2 -Wall -Wextra -Wpedantic \
  $(pkg-config --cflags gtk4 glib-2.0 gio-2.0 json-glib-1.0) \
  -o umicom \
  src/*.c \
  $(pkg-config --libs gtk4 glib-2.0 gio-2.0 json-glib-1.0)

macOS (Homebrew)
----------------
brew install gtk4 glib json-glib pkg-config
clang -std=c17 -O2 -Wall -Wextra -Wpedantic \
  $(pkg-config --cflags gtk4 glib-2.0 gio-2.0 json-glib-1.0) \
  -o umicom \
  src/*.c \
  $(pkg-config --libs gtk4 glib-2.0 gio-2.0 json-glib-1.0)

Notes
-----
- Define HAVE_GSV5 if GtkSourceView-5 is available and you want syntax highlight:
  add 'gtksourceview-5' to pkg-config cflags/libs and compile editor with that flag.
- On Windows, ripgrep binary discovery checks USTUDIO_RG env var first.
- For reproducible builds, consider Meson:
  meson setup build && meson compile -C build
*/
