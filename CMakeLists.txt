# ---------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Top-level CMake configuration
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)

# ------------------------------------------------------------------------------
# Umicom Studio IDE — top-level CMake
# This file intentionally uses GLOB_RECURSE with a small exclude list so we
# can keep a large tree sane while we finish consolidating modules. If you
# later want pinned file lists, that’s easy to swap in.
# ------------------------------------------------------------------------------
project(umicom-studio-ide C)  # C-ONLY (toolchain friendly on MSYS2/Clang)

# C standard / common flags
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)        # match -std=gnu17 you’ve been using

if(MSVC)
  # Keep sane warnings under MSVC for contributors who use it
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra -Wno-deprecated-declarations)
  add_compile_definitions(__USE_MINGW_ANSI_STDIO=1 LIBDEFLATE_DLL)
  # Match the build lines you’ve been using
  add_compile_options(-mfpmath=sse -msse -msse2)
  # Faster links with clang/lld (works fine on MSYS2)
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
  endif()
endif()

# ------------------------------------------------------------------------------
# Dependencies via pkg-config (MSYS2/MinGW has these)
# ------------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

# Core GTK/GLib stack
pkg_check_modules(GTK4        REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(PANGO       REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(GDK_PIXBUF  REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO       REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(HARFBUZZ    REQUIRED IMPORTED_TARGET harfbuzz)
pkg_check_modules(GRAPHENE    REQUIRED IMPORTED_TARGET graphene-1.0)
pkg_check_modules(GLIB        REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT     REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO         REQUIRED IMPORTED_TARGET gio-2.0)

# JSON + Curl (used by core store + LLM HTTP)
pkg_check_modules(JSON_GLIB   REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(CURL        REQUIRED IMPORTED_TARGET libcurl)

# Networking for chat pane (libsoup 3, GTK4 era)
pkg_check_modules(SOUP3       REQUIRED IMPORTED_TARGET libsoup-3.0)

# ------------------------------------------------------------------------------
# Source layout & includes
# ------------------------------------------------------------------------------
# Build mode: full app or smoke test
option(USTUDIO_SMOKE "Build minimal smoke test instead of the full app" OFF)

# Collect every .c under src/
file(GLOB_RECURSE UMI_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# If we’re not explicitly building the tiny smoke test, exclude its driver
if (NOT USTUDIO_SMOKE)
  list(FILTER UMI_SOURCES EXCLUDE REGEX ".*/src/gtk_smoke\.c$")
endif()

# Exclude deprecated/duplicate implementations to prevent duplicate symbols
set(UMI_EXCLUDE_REGEX
  ".*/src/umi_log\.c$"                # legacy duplicate (real one lives in src/core)
  ".*/src/build_queue\.c$"            # legacy duplicate (real one lives in src/build)
  ".*/src/build_system\.c$"           # legacy duplicate (real one lives in src/build)
  ".*/src/diagnostic_parsers\.c$"     # legacy duplicate (real one lives in src/build)
  ".*/src/app\.c$"                    # legacy duplicate (real one lives in src/gui)
  ".*/src/palette\.c$"                # LEGACY: replaced by src/command_palette.c
  ".*/src/problem_list_shim\.c$"      # temporary shim (should NOT ship in final app)
)

foreach(rx IN LISTS UMI_EXCLUDE_REGEX)
  list(FILTER UMI_SOURCES EXCLUDE REGEX "${rx}")
endforeach()

# Helpful messages (you were printing these earlier)
message(STATUS "umicom-studio sources: ${CMAKE_CURRENT_SOURCE_DIR}")
list(LENGTH UMI_SOURCES NUM_C)
message(STATUS "Number of C sources: ${NUM_C}")

# ------------------------------------------------------------------------------
# Target
# ------------------------------------------------------------------------------
add_executable(ustudio ${UMI_SOURCES})

# Public/Private include paths used throughout the tree.
# Keep all module subfolders so headers resolve regardless of local includes.
target_include_directories(ustudio PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/search"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/llm"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/providers"
)

# Link (order doesn’t strictly matter with lld, but keep it tidy)
target_link_libraries(ustudio PRIVATE
  PkgConfig::GTK4
  PkgConfig::PANGO
  PkgConfig::GDK_PIXBUF
  PkgConfig::CAIRO
  PkgConfig::HARFBUZZ
  PkgConfig::GRAPHENE
  PkgConfig::GLIB
  PkgConfig::GOBJECT
  PkgConfig::GIO
  PkgConfig::JSON_GLIB
  PkgConfig::CURL
  PkgConfig::SOUP3
)

# Optional feature define if you guard code paths on libsoup presence
target_compile_definitions(ustudio PRIVATE HAVE_LIBSOUP=1)

# Build include shim for GTK widgets (safety across TUs)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(ustudio PRIVATE -include "${CMAKE_SOURCE_DIR}/include/gtk_all.h")
endif()

# On Windows, ensure Win32 libs are pulled in and pick the Unicode entry point.
# GTK’s pkg-config typically sets GUI subsystem via -mwindows; -municode makes the CRT expect wWinMain.
if(WIN32)
  target_link_libraries(ustudio PRIVATE
    kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32
  )
  target_link_options(ustudio PRIVATE -municode)
endif()

# Default build type for local dev
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()
