# CMake build for Umicom Studio IDE
# -----------------------------------------------------------------------------
# This file is a *complete* and self‑contained CMake configuration intended
# for MSYS2/MinGW on Windows, but it also works on Linux/macOS with pkg-config.
#
# Highlights:
#   • Option USIDE_DEV_CONSOLE toggles Console vs GUI subsystem on Windows.
#     - Console is handy during development to see GLib/printf logs.
#     - GUI uses wWinMain from src/win_launcher.c.
#   • Adds include paths for all subfolders and pkg‑config dependencies.
#   • Excludes scratch/test sources (e.g. src/gtk_smoke.c) from the main app.
#   • Uses -Wall/-Wextra and keeps deprecated GTK warnings visible by default.
#
# Usage (MSYS2 MinGW64 shell or PowerShell with PATH set to msys64\mingw64\bin):
#   cmake -S . -B build-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSIDE_DEV_CONSOLE=ON
#   ninja -C build-ninja -j1
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)
project(ustudio C)

# ---- Options -----------------------------------------------------------------
option(USIDE_DEV_CONSOLE "Build with a console (helps to see logs during dev)" ON)

# ---- Language / Warnings -----------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# ---- Dependencies via pkg-config --------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(USIDE_PKG REQUIRED
  gtk4
  pango
  gdk-pixbuf-2.0
  cairo
  harfbuzz
  graphene-1.0
  glib-2.0
  gobject-2.0
  gio-2.0
  json-glib-1.0
  libsoup-3.0
  libcurl
)

# ---- Source discovery --------------------------------------------------------
# Grab all .c files under src/, then *exclude* scratch/test entry points that may
# define their own main() or are not meant to ship with the IDE.
file(GLOB_RECURSE USIDE_ALL_C "${CMAKE_SOURCE_DIR}/src/*.c")

# Remove experimental/demo sources from the main target if present:
foreach(BAD IN ITEMS
  "${CMAKE_SOURCE_DIR}/src/gtk_smoke.c"
)
  list(REMOVE_ITEM USIDE_ALL_C "${BAD}")
endforeach()

# Make sure the Windows launcher is always compiled on Windows to provide
# wWinMain / wmain shims when we use the GUI subsystem.
if(WIN32)
  list(APPEND USIDE_ALL_C "${CMAKE_SOURCE_DIR}/src/win_launcher.c")
endif()

# ---- Target ------------------------------------------------------------------
# On Windows, use WIN32 for GUI subsystem when console is OFF; otherwise build a
# normal console exe. On non-Windows, WIN32 is ignored anyway.
if(WIN32 AND NOT USIDE_DEV_CONSOLE)
  add_executable(ustudio WIN32 ${USIDE_ALL_C})
else()
  add_executable(ustudio ${USIDE_ALL_C})
endif()

# Includes: project headers + pkg-config
target_include_directories(ustudio PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/editor"
  "${CMAKE_SOURCE_DIR}/src/gui"
  "${CMAKE_SOURCE_DIR}/src/core"
  "${CMAKE_SOURCE_DIR}/src/panes"
  "${CMAKE_SOURCE_DIR}/src/plugins"
  "${CMAKE_SOURCE_DIR}/src/plugins/transpile"
  "${CMAKE_SOURCE_DIR}/src/build"
  "${CMAKE_SOURCE_DIR}/src/llm"
  "${CMAKE_SOURCE_DIR}/src/llm/providers"
  "${CMAKE_SOURCE_DIR}/src/search"
  "${CMAKE_SOURCE_DIR}/src/ui"
  ${USIDE_PKG_INCLUDE_DIRS}
)

target_compile_options(ustudio PRIVATE
  -Wall -Wextra -Wno-missing-field-initializers
)

# Libraries discovered by pkg-config
target_link_libraries(ustudio PRIVATE ${USIDE_PKG_LIBRARIES})

# Propagate any extra flags from pkg-config (MSYS2 typically needs these)
target_compile_options(ustudio PRIVATE ${USIDE_PKG_CFLAGS_OTHER})
target_link_options(ustudio PRIVATE ${USIDE_PKG_LDFLAGS_OTHER})

# Windows specific: make sure we use Unicode entry points and safe defaults.
if(MINGW)
  target_compile_definitions(ustudio PRIVATE UNICODE _UNICODE)
  target_compile_options(ustudio PRIVATE -municode)
  # Hardened linker flags are generally OK
  target_link_options(ustudio PRIVATE -Wl,--dynamicbase -Wl,--nxcompat)
endif()

# Install (optional)
install(TARGETS ustudio RUNTIME DESTINATION bin)
