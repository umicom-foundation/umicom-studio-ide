#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
#
# PURPOSE (high level):
#   - Describe how to build the Umicom Studio IDE using CMake in a way that is
#     easy for *new developers* to understand and modify.
#   - Keep the project *loosely coupled*: each module (core, editor, panes,
#     ui/resources, etc.) is compiled as plain C files without tight link-time
#     dependencies on optional features.
#   - Avoid XML resource pipelines: embed icons/splash directly via C sources
#     (see src/ui/resources/icon.c and src/ui/resources/splash.c).
#
# READER NOTE (for beginners):
#   - CMake is a *declarative* build language: we "declare" what to build and
#     with what settings; CMake then generates Ninja/Make/MSBuild files that
#     actually compile the code.
#   - Most commands look like: `command(NAME ARGS...)`.
#   - Lines starting with `#` are comments (like this one); they do not affect
#     the build but *explain* what each line does.
#
# DESIGN RULES (that we keep repeating in each module):
#   1) Loosely coupled: no module should #include headers from unrelated
#      modules unless it is *really* necessary. Cross-module calls should be
#      optional and guarded (weak symbols or function pointers).
#   2) Pure C where possible: we avoid code-gen and XML pipelines unless there
#      is a compelling reason; that keeps builds portable and simple.
#   3) Keep comments abundant (>70%) and credit header present in every file.
#
# Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
#-----------------------------------------------------------------------------

# The `cmake_minimum_required` line tells CMake which feature set we rely on.
# We use 3.24 for good generator expressions and CONFIGURE_DEPENDS on GLOB.
cmake_minimum_required(VERSION 3.24)

# `project()` declares the project name and primary language(s). Using C only.
project(umicom-studio-ide LANGUAGES C)

# `option()` defines a boolean cache variable that users can set at configure
# time. `USIDE_GUI` controls whether we link as a GUI app (Windows subsystem)
# or as a console app. Default is ON for the normal GUI IDE.
# ---- 3) Human-readable version (optional but nice to have) -------------------
# SemVer triplet you can later embed in the binary or print in About dialog.
set(UMICOM_STUDIO_IDE_VERSION_MAJOR 0)
set(UMICOM_STUDIO_IDE_VERSION_MINOR 9)
set(UMICOM_STUDIO_IDE_VERSION_PATCH 0)
set(UMICOM_STUDIO_IDE_VERSION
    "${UMICOM_STUDIO_IDE_VERSION_MAJOR}.${UMICOM_STUDIO_IDE_VERSION_MINOR}.${UMICOM_STUDIO_IDE_VERSION_PATCH}")


# ---- 4) Build toggles (you pass -DNAME=ON/OFF to cmake) ----------------------
# ON means the default is enabled unless the user overrides it.

# When ON (default) we build as a GUI app (no console window on Windows); when
# OFF we build as a console app (handy for debug logs). Same single binary,
# just a different Windows subsystem setting.

option(USIDE_GUI "Build GUI subsystem (OFF builds console subsystem)" ON)

# Set the C language standard for all targets in this project. We use C23
# features (available as `gnu2x` in MinGW) but keep portability in mind.
set(CMAKE_C_STANDARD 23)               # request C23 where available
set(CMAKE_C_STANDARD_REQUIRED ON)      # require the standard (do not fallback)
set(CMAKE_C_EXTENSIONS ON)             # allow GNU extensions (helps on MinGW)

# For beginners: build types control optimizations and debug symbols.
# Common values: Debug, Release, RelWithDebInfo, MinSizeRel.
# If the user didn't pass one, default to RelWithDebInfo (optimized with
# symbols, useful for profiling and crash reports).
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# We use pkg-config to discover GTK4, GLib and friends on all platforms.
# `find_package(PkgConfig)` gives us the `pkg_check_modules()` function.
find_package(PkgConfig REQUIRED)  # REQUIRED = fail early if missing

# Helper macro to check a package and expose INCLUDE/LIB/CFLAGS variables.
# We keep each dependency as its own `pkg_check_modules()` call so the output
# is readable to new contributors.
pkg_check_modules(GLIB        REQUIRED glib-2.0)        # base utility library
pkg_check_modules(GOBJECT     REQUIRED gobject-2.0)     # object system
pkg_check_modules(GIO         REQUIRED gio-2.0)         # I/O and app framework
pkg_check_modules(JSON_GLIB   REQUIRED json-glib-1.0)   # JSON parsing
pkg_check_modules(SOUP3       REQUIRED libsoup-3.0)     # HTTP client (GTK stack)
pkg_check_modules(CURL        REQUIRED libcurl)         # Optional network usage
pkg_check_modules(GTK4        REQUIRED gtk4)            # the GUI toolkit
pkg_check_modules(PIXBUF      REQUIRED gdk-pixbuf-2.0)  # image loaders
pkg_check_modules(CAIRO       REQUIRED cairo)           # 2D graphics
pkg_check_modules(PANGO       REQUIRED pango)           # text layout
pkg_check_modules(HARFBUZZ    REQUIRED harfbuzz)        # shaping engine
pkg_check_modules(GRAPHENE    REQUIRED graphene-1.0)    # math for GTK
# GtkSourceView is optional (code editor widget). We do not fail if missing.
pkg_check_modules(SOURCEVIEW5 OPTIONAL gtksourceview-5)

# New pure-C resources (icons and splash) — no XML resource files. We just
# compile plain .c files that embed small images / draw a splash.
#   - icon.c     : provides a small in-memory XPM fallback and helpers to
#                  load PNG/SVG at runtime when available.
#   - splash.c   : draws a tiny splash window in pure GTK4 C.
# You can safely remove these files later if you change approach.
set(UI_RESOURCE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/icon.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/splash.c
)

# We gather the project sources. For a newcomer it is simpler to list *folders*
# and let CMake glob *.c files. We use CONFIGURE_DEPENDS so CMake regenerates
# when new files are added (not perfect, but beginner-friendly).
# NOTE: If you prefer explicit file lists, replace this block with a manual
# list — it’s more robust on large teams.
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.c"          # core app logic
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build/*.c"         # build system
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.c"        # editor glue
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/**/*.c"        # GUI modules (app, panes)
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/**/*.c"      # output/problems/chat
  "${CMAKE_CURRENT_SOURCE_DIR}/src/search/*.c"        # ripgrep integration
  "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/**/*.c"        # LLM lab
# ---- 8) Optional: XML gresource (kept OFF by default) -----------------------
# If you later decide you *want* gresource, drop an XML manifest at this path
# and pass -DUSIDE_USE_GRESOURCE=ON. Otherwise, we rely on pure-C assets in
# src/ui/icons.c and src/ui/splash.c (no XML involved).
#  $<$<BOOL:${USIDE_USE_GRESOURCE}>:
#    "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/resources.c"
#  >
  # ---- 9) Always include pure-C resources (no XML pipeline) ----------------
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/*.c"  # pure-C resources
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/**/*.c"       # utilities (fs, sys)
  "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"            # single entry point
)

# Create the executable target that will contain *all* project code.
# This is the “thing” we ultimately run (an .exe on Windows, or a binary on
# Linux/macOS). The variable name is the project name — consistent and simple.
add_executable(${PROJECT_NAME} ${USIDE_SOURCES} ${UI_RESOURCE_SOURCES})

# Include directories tell the compiler where to find headers when a source
# does `#include "something.h"`. We add both module "include" folders and
# the pkg-config include paths (GTK/GLib headers).
target_include_directories(${PROJECT_NAME} PRIVATE
# ---- 9) Friendly failure if nothing matched (helps new contributors) --------
if(NOT USIDE_SOURCES)
  message(FATAL_ERROR "No sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()


# ---- 10) Project include paths (so #include \"foo.h\" just works) -----------
# We *explicitly* add the most common header roots. This is teaching material:
# reading this list tells you where headers live in the tree.
# You can later switch to `target_include_directories(... INTERFACE src)` if
# you prefer a single umbrella include path (less typing, but less explicit).
${CMAKE_CURRENT_SOURCE_DIR}                        # allow #include "src/.."
  ${CMAKE_CURRENT_SOURCE_DIR}/src                   # generic umbrella include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/include           # top-level headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include      # core public headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/build/include     # build headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include    # editor headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include   # app headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/prefs/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/chat/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/search/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include        # UI public headers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/platform/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/git/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/watchers/include

  # pkg-config discovered include directories (GTK/GLib & friends):
  ${GLIB_INCLUDE_DIRS}
  ${GOBJECT_INCLUDE_DIRS}
  ${GIO_INCLUDE_DIRS}
  ${JSON_GLIB_INCLUDE_DIRS}
  ${SOUP3_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
  ${GTK4_INCLUDE_DIRS}
  ${PIXBUF_INCLUDE_DIRS}
  ${CAIRO_INCLUDE_DIRS}
  ${PANGO_INCLUDE_DIRS}
  ${HARFBUZZ_INCLUDE_DIRS}
  ${GRAPHENE_INCLUDE_DIRS}
  ${SOURCEVIEW5_INCLUDE_DIRS}   # optional; may be empty
)

# Compiler flags: keep them visible so beginners learn what they do.
# -Wall -Wextra -Wpedantic : typical “be strict” warnings
# -Wno-unused-parameter    : many GTK callbacks have unused parameters
# -DNOMINMAX               : Windows headers sometimes define min/max macros
# -D_CRT_SECURE_NO_WARNINGS: silence MSVC warnings about fopen, etc.
# -Dgtk_widget_destroy=g_object_unref : prefer unref over destroy on GTK4
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang>:
    -Wall -Wextra -Wpedantic -Wno-unused-parameter
    -mfpmath=sse -msse -msse2
  >
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  NOMINMAX
  _CRT_SECURE_NO_WARNINGS
  gtk_widget_destroy=g_object_unref
  $<$<BOOL:${SOURCEVIEW5_FOUND}>:HAVE_GTKSOURCEVIEW5>
  $<$<BOOL:${USIDE_GUI}>:USIDE_GUI=1>
)

# Linker subsystem selection for Windows:
# - When building the GUI variant we link with the "windows" subsystem so
#   the app does not open a console window.
# - When building the console variant (dev tools) we link with "console".
# On non-Windows platforms, these flags are ignored safely.
if(MSVC)
  # MSVC handles subsystem differently; we keep defaults (GUI == /SUBSYSTEM:WINDOWS).
  # You can extend this branch if you later support MSVC builds.
else()
  target_link_options(${PROJECT_NAME} PRIVATE
    $<$<AND:$<BOOL:${USIDE_GUI}>,$<STREQUAL:$<C_COMPILER_ID>,GNU>>:-mwindows -Wl,--subsystem,windows>
    $<$<AND:$<NOT:$<BOOL:${USIDE_GUI}>>,$<STREQUAL:$<C_COMPILER_ID>,GNU>>:-Wl,--subsystem,console>
  )
endif()

# Link libraries discovered via pkg-config. We add them as PRIVATE because
# the executable does not "expose" these libraries to anything else.
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${GLIB_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GIO_LIBRARIES}
  ${JSON_GLIB_LIBRARIES}
  ${SOUP3_LIBRARIES}
  ${CURL_LIBRARIES}
  ${GTK4_LIBRARIES}
  ${PIXBUF_LIBRARIES}
  ${CAIRO_LIBRARIES}
  ${PANGO_LIBRARIES}
  ${HARFBUZZ_LIBRARIES}
  ${GRAPHENE_LIBRARIES}
  ${SOURCEVIEW5_LIBRARIES}  # empty if not found — that’s okay
)

# Some pkg-config packages emit extra compiler flags that are not include
# paths (like `-pthread`). We add them to the compile options so new devs
# don’t need to hunt them down.
target_compile_options(${PROJECT_NAME} PRIVATE
  ${GLIB_CFLAGS_OTHER}
  ${GOBJECT_CFLAGS_OTHER}
  ${GIO_CFLAGS_OTHER}
  ${JSON_GLIB_CFLAGS_OTHER}
  ${SOUP3_CFLAGS_OTHER}
  ${CURL_CFLAGS_OTHER}
  ${GTK4_CFLAGS_OTHER}
  ${PIXBUF_CFLAGS_OTHER}
  ${CAIRO_CFLAGS_OTHER}
  ${PANGO_CFLAGS_OTHER}
  ${HARFBUZZ_CFLAGS_OTHER}
  ${GRAPHENE_CFLAGS_OTHER}
  ${SOURCEVIEW5_CFLAGS_OTHER}
)

# On Windows we often need to ensure the runtime finds the GTK DLLs at run
# time. That is an environment/runtime concern (PATH), not a link concern,
# so we do NOT hard-code absolute DLL paths here. The developer sets PATH
# in their shell (as you already do).
# If you later want to produce a redistributable bundle, add a CPack step
# or a post-build script that copies the GTK runtime next to the EXE.

# Friendly summary messages to help beginners see what's being built.
message(STATUS "USIDE_GUI: ${USIDE_GUI} (ON=GUI subsystem, OFF=console subsystem)")
message(STATUS "GtkSourceView 5 found: ${SOURCEVIEW5_FOUND}")

# INSTALL step (optional): allow `cmake --install <builddir>` to copy the
# executable to a staging area. Adjust DESTINATION as you prefer.
# Optional native app icon (best for Windows taskbar/Alt-Tab):
  # If you put a `res/umicom.ico` and `win/appicon.rc` in the tree, we add it.
  # The .rc is tiny:
  #   // win/appicon.rc
  #   // 1 ICON "res/umicom.ico"
  # This is NOT XML and keeps your “pure C” runtime; it’s just a build-time
  # resource for the executable.
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/win/appicon.rc")
    target_sources(umicom-studio-ide PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/win/appicon.rc")
  endif()
# ---- 15) (Optional) install step (copy the exe to bin/) ---------------------
# install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
# ---- Appendix: Teaching notes ------------------------------------------------
#[[
BAD EXAMPLE (kept here to teach what NOT to do):
  add_definitions(-IC:/some/include/path)
That injects an *include directory* using a macro mechanism. Wrong tool.
Correct approach (what we used above):
  target_include_directories(umicom-studio-ide PRIVATE <paths...>)

Palette / Theme notes:
  src/gui/theme/palette.c is excluded by default. Pass -DUSIDE_EXCLUDE_PALETTE=OFF
  to include it in the build once you’re comfortable.

Splash / Icons:
  The pure-C splash/icon helpers live under src/ui/. No XML involved. If you
  want GTK gresource later, flip -DUSIDE_USE_GRESOURCE=ON and add the XML.

Windows icon:
  Create res/umicom.ico (multi-size 16/32/48/64/128). A quick way is:
    magick logo.svg -define icon:auto-resize=16,32,48,64,128 res/umicom.ico
  Then add a tiny win/appicon.rc with: 1 ICON "res/umicom.ico"
  CMake will add it automatically if the file exists.
]]
#---------------------------------------------------------------------------*/