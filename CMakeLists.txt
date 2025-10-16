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
#   - CMake is a *declarative* build language.
#   - Lines starting with `#` are comments (like this one).#     with what settings; CMake then generates Ninja/Make/MSBuild files that
#     actually compile the code.
#   - Most commands look like: `command(NAME ARGS...)`.
#   - Lines starting with `#` are comments (like this one); they do not affect
#     the build but *explain* what each line does.
#
# DESIGN RULES FOR THIS PROJECT:
#   1) Loosely coupled modules:
#      Each subfolder (core, build, editor, gui, panes, search, util, ui)
#      should be compilable without hard dependencies on optional features.
#      Cross-module calls should be guarded and optional.
#   2) Pure C where possible:
#      We prefer direct C sources for assets (icons, splash) over XML/gresource.
#   3) Comments everywhere:
#      This file is a tutorial. It favors readability and clear guidance.
#
# CREDIT:
#   Created by: Umicom Foundation | Developer: Sammy Hegab
#   Maintainers: The Umicom Foundation team and community contributors
#   License: MIT
#-----------------------------------------------------------------------------

# cmake_minimum_required() tells CMake which feature set we expect.
# We pick 3.24 so we can rely on modern generator expressions and
# CONFIGURE_DEPENDS behavior for file globbing on changes.
cmake_minimum_required(VERSION 3.24)

# project() declares the project name and primary languages.
# We only use "C" here. If you later add C++, list it too.
project(umicom-studio-ide LANGUAGES C)

# Human-readable version (optionally printed in About dialog).
# We also inject it as compile definitions so code can use it.set(UMICOM_STUDIO_IDE_VERSION_MAJOR 0)
set(UMICOM_STUDIO_IDE_VERSION_MAJOR 0)
set(UMICOM_STUDIO_IDE_VERSION_MINOR 9)
set(UMICOM_STUDIO_IDE_VERSION_PATCH 0)
set(UMICOM_STUDIO_IDE_VERSION
    "${UMICOM_STUDIO_IDE_VERSION_MAJOR}.${UMICOM_STUDIO_IDE_VERSION_MINOR}.${UMICOM_STUDIO_IDE_VERSION_PATCH}")

# Build toggle to choose Windows subsystem:
#  - ON  -> build as GUI subsystem (no console window pop-up)
#  - OFF -> build as console subsystem (handy for development logs)
# Pass -DUSIDE_GUI=OFF to get console builds.
# When ON (default) we build as a GUI app (no console window on Windows); when
# OFF we build as a console app (handy for debug logs). Same single binary,
# just a different Windows subsystem setting.

option(USIDE_GUI "Build GUI subsystem (OFF builds console subsystem)" ON)

# Choose the C language standard.
# We want modern C (C23 if supported). On MinGW this maps to gnu2x.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON) # allow GNU extensions (useful on MinGW)

# Default build type if the user didn't pass one (-DCMAKE_BUILD_TYPE=...).
# RelWithDebInfo = optimized + debug symbols, good for crash logs/profiling.
# For beginners: build types control optimizations and debug symbols.
# Common values: Debug, Release, RelWithDebInfo, MinSizeRel.
# If the user didn't pass one, default to RelWithDebInfo (optimized with
# symbols, useful for profiling and crash reports).



# ------- ImageMagick-based asset generation (optional but nice) ---------------
find_program(MAGICK_EXE NAMES magick)

if (MAGICK_EXE)
  set(UMI_SVG_ICON ${CMAKE_SOURCE_DIR}/assets/umicom-icon.svg)
  set(UMI_SVG_LOGO ${CMAKE_SOURCE_DIR}/assets/umicom-logo.svg)
  set(UMI_WIN_DIR  ${CMAKE_SOURCE_DIR}/win)
  set(UMI_ICO_OUT  ${UMI_WIN_DIR}/umicom.ico)
  set(UMI_PNG_OUT  ${UMI_WIN_DIR}/umicom-logo.png)

  add_custom_command(
    OUTPUT ${UMI_ICO_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${UMI_WIN_DIR}
    # --- FIX: proactively remove any stale/locked target before writing ---
    COMMAND ${CMAKE_COMMAND} -E rm -f ${UMI_ICO_OUT}
    COMMAND ${MAGICK_EXE} -background none -density 1024
            ${UMI_SVG_ICON}
            -define icon:auto-resize=256,128,96,64,48,32,24,20,16
            ${UMI_ICO_OUT}
    DEPENDS ${UMI_SVG_ICON}
    COMMENT "Generating umicom.ico from assets/umicom-icon.svg"
    VERBATIM
  )

  add_custom_command(
    OUTPUT ${UMI_PNG_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${UMI_WIN_DIR}
    # keep the pattern consistent (helps with R/O attribute issues)
    COMMAND ${CMAKE_COMMAND} -E rm -f ${UMI_PNG_OUT}
    COMMAND ${MAGICK_EXE} -background none -density 512
            ${UMI_SVG_LOGO}
            -resize 800
            ${UMI_PNG_OUT}
    DEPENDS ${UMI_SVG_LOGO}
    COMMENT "Generating umicom-logo.png from assets/umicom-logo.svg"
    VERBATIM
  )

  add_custom_target(umicom_assets ALL DEPENDS ${UMI_ICO_OUT} ${UMI_PNG_OUT})
endif()

# Default build type if user didn’t pass -DCMAKE_BUILD_TYPE=...
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

# ------- Dependencies (via pkg-config) ---------------------------------------
# We use pkg-config to locate GTK4/GLib and related libraries on all platforms.
# This provides the `pkg_check_modules()` function we call below.
find_package(PkgConfig REQUIRED) # REQUIRED => fail early with a clear message.

# Each dependency is checked separately for maximum clarity in the logs.
# The _INCLUDE_DIRS / _LIBRARIES / _CFLAGS_OTHER variables are populated.
# Helper macro to check a package and expose INCLUDE/LIB/CFLAGS variables.
# We keep each dependency as its own `pkg_check_modules()` call so the output
# is readable to new contributors.pkg_check_modules(GLIB        REQUIRED glib-2.0)
pkg_check_modules(GOBJECT     REQUIRED gobject-2.0)
pkg_check_modules(GIO         REQUIRED gio-2.0)
pkg_check_modules(JSON_GLIB   REQUIRED json-glib-1.0)
pkg_check_modules(SOUP3       REQUIRED libsoup-3.0)
pkg_check_modules(CURL        REQUIRED libcurl)
pkg_check_modules(GTK4        REQUIRED gtk4)
pkg_check_modules(PIXBUF      REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(CAIRO       REQUIRED cairo)
pkg_check_modules(PANGO       REQUIRED pango)
pkg_check_modules(HARFBUZZ    REQUIRED harfbuzz)
pkg_check_modules(GRAPHENE    REQUIRED graphene-1.0)
# GtkSourceView 5 is optional (code editor widget with syntax highlighting).
# We use it if available, but can build without it (plain text editor).
# If you want to make it required, change to REQUIRED here.
pkg_check_modules(SOURCEVIEW5 REQUIRED gtksourceview-5 QUIET)

# --- Collect source files -----------------------------------------------------
# We use GLOB_RECURSE to make the file list newbie-friendly (it auto-picks
# new .c files you add). On very large teams you may prefer hand-listing
# sources for stability, but here the glob is convenient for fast iteration.
#
# IMPORTANT:
#   We do *NOT* glob src/ui/resources/*.c here to avoid duplicate additions.
#   Those files are added explicitly via UI_RESOURCE_SOURCES below.
# We gather the project sources. For a newcomer it is simpler to list *folders*
# and let CMake glob *.c files. We use CONFIGURE_DEPENDS so CMake regenerates
# when new files are added (not perfect, but beginner-friendly).
# NOTE: If you prefer explicit file lists, replace this block with a manual
# list — it’s more robust on large teams.
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.c"
)

# Optional: legacy pure-C resource bundle (as in your logs).
message(STATUS "Including legacy ustudio_resources.c (pure C resource bundle).")

# Windows icon resource
set(UMI_WIN_RC "${CMAKE_SOURCE_DIR}/win/umicom.rc")
if (EXISTS "${UMI_WIN_RC}")
  message(STATUS "Including Windows icon resource: ${UMI_WIN_RC}")
  list(APPEND USIDE_SOURCES "${UMI_WIN_RC}")
endif()

# ------- Target ---------------------------------------------------------------
# Create the executable target that contains *all* project code.
# NOTE: We append UI_RESOURCE_SOURCES explicitly to avoid duplicates
# with the glob above.

add_executable(${PROJECT_NAME}
  ${USIDE_SOURCES}
)

# Ensure assets exist before compile/link (if ImageMagick was found).
if (TARGET umicom_assets)
  add_dependencies(${PROJECT_NAME} umicom_assets)
endif()
# --- Include directories (where headers are found) ----------------------------
# We list the public include roots for each module so `#include "icon.h"` works
# without cross-module paths in your C files (you asked for short includes).

# ------- Include paths --------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/include

  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/build/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include

  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/prefs/include

  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/chat/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems/include

  ${CMAKE_CURRENT_SOURCE_DIR}/src/search/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include

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
  ${SOURCEVIEW5_INCLUDE_DIRS} # optional; may be empty
)

# --- Compiler flags & definitions --------------------------------------------
# Teach newcomers about the common warning settings and convenience macros.
# Compiler flags: keep them visible so beginners learn what they do.
# -Wall -Wextra -Wpedantic : typical “be strict” warnings
# -Wno-unused-parameter    : many GTK callbacks have unused parameters
# -DNOMINMAX               : Windows headers sometimes define min/max macros
# -D_CRT_SECURE_NO_WARNINGS: silence MSVC warnings about fopen, etc.
# -Dgtk_widget_destroy=g_object_unref : prefer unref over destroy on GTK4
# -DUSIDE_GUI=1            : so code can check if GUI or console build
# -DUMICOM_VERSION_MAJOR  : so code can use version info
# -DUMICOM_VERSION_MINOR  : so code can use version info
# -DUMICOM_VERSION_PATCH  : so code can use version info
# We use generator expressions to apply some flags only on certain compilers.

target_compile_definitions(${PROJECT_NAME} PRIVATE
  NOMINMAX
  _CRT_SECURE_NO_WARNINGS
  USIDE_GUI=$<IF:$<BOOL:${USIDE_GUI}>,1,0>
  UMICOM_VERSION_MAJOR=${UMICOM_STUDIO_IDE_VERSION_MAJOR}
  UMICOM_VERSION_MINOR=${UMICOM_STUDIO_IDE_VERSION_MINOR}
  UMICOM_VERSION_PATCH=${UMICOM_STUDIO_IDE_VERSION_PATCH}
  # Prefer unref in GTK4
  gtk_widget_destroy=g_object_unref
)
# --- Windows subsystem selection (GUI vs console) ----------------------------
# MinGW/GCC honors -mwindows/--subsystem flags. On other platforms this is
# ignored safely. Keep it generator-expression guarded for clarity.
# Linker subsystem selection for Windows:
# - When building the GUI variant we link with the "windows" subsystem so
#   the app does not open a console window.
# - When building the console variant (dev tools) we link with "console".
# On non-Windows platforms, these flags are ignored safely.

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter -mfpmath=sse -msse -msse2)
endif()

# ------- Link libraries -------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
  PkgConfig::GLIB
  PkgConfig::GOBJECT
  PkgConfig::GIO
  PkgConfig::JSON_GLIB
  PkgConfig::SOUP3
  PkgConfig::CURL
  PkgConfig::GTK4
  PkgConfig::PIXBUF
  PkgConfig::CAIRO
  PkgConfig::PANGO
  PkgConfig::HARFBUZZ
  PkgConfig::GRAPHENE
  ${SOURCEVIEW5_LIBRARIES}
)

# Windows: build as GUI subsystem when requested
# --- Optional native Windows icon at build time (not XML) --------------------
# If you create a Windows .ico and a tiny .rc file, we can embed the taskbar
# icon directly. This is *build-time* only and keeps runtime pure-C.
#   res/umicom.ico   (multi-size 16/32/48/64/128)
# On Windows we often need to ensure the runtime finds the GTK DLLs at run
# time. That is an environment/runtime concern (PATH), not a link concern,
# so we do NOT hard-code absolute DLL paths here. The developer sets PATH
# in their shell (as you already do).
# If you later want to produce a redistributable bundle, add a CPack step
# or a post-build script that copies the GTK runtime next to the EXE.

# Friendly summary messages to help beginners see what's being built.
# If the user created res/umicom.ico and win/appicon.rc, we add it to the
# build. The .rc file is simple to create; it just references the .ico:
# INSTALL step (optional): allow `cmake --install <builddir>` to copy the
# executable to a staging area. Adjust DESTINATION as you prefer.
# Optional native app icon (best for Windows taskbar/Alt-Tab):
  # If you put a `res/umicom.ico` and `win/appicon.rc` in the tree, we add it.
  # The .rc is tiny:
  #   // win/appicon.rc
  #   // 1 ICON "res/umicom.ico"
  # This is NOT XML and keeps your “pure C” runtime; it’s just a build-time
  # resource for the executable.

#   win/appicon.rc   with the single line:  1 ICON "res/umicom.ico"
#  --- ADDED: use your actual resource file path/name (win/umicom.rc) ---

if (WIN32 AND USIDE_GUI)
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)
  message(STATUS "USIDE_GUI: ON (ON=GUI subsystem, OFF=console)")
endif()

# Version note
# --- Install step (optional) --------------------------------------------------
# This enables: `cmake --install <build-dir>` to copy the exe into bin/.
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# --- Friendly summary messages -----------------------------------------------
message(STATUS "USIDE_GUI: ${USIDE_GUI} (ON=GUI subsystem, OFF=console)")
message(STATUS "GtkSourceView 5 found: ${SOURCEVIEW5_FOUND}")
message(STATUS "Version: ${UMICOM_STUDIO_IDE_VERSION}")

message(STATUS "Version: ${UMICOM_STUDIO_IDE_VERSION}")

# --- Teaching notes (kept as comment appendix) --------------------------------
#[[
BAD EXAMPLE (don’t do this):
  add_definitions(-IC:/some/include/path)
That misuses preprocessor macros to push include paths. Prefer:
  target_include_directories(${PROJECT_NAME} PRIVATE <paths...>)

Pure-C resources:
  We compile src/ui/resources/icon.c and splash.c directly. No XML/gresource.
  If you later want gresource, flip -DUSIDE_USE_GRESOURCE=ON and add a step
  to generate a .c file from your XML.

Bundling GTK on Windows:
  At development time, keep C:\msys64\mingw64\bin on PATH before you run.
  For distribution, use a packaging script (e.g. CPack or your own) that
  copies the needed DLLs next to the executable.

Loose coupling:
  Avoid #including headers across unrelated modules unless necessary.
  Prefer storing cross-module pointers via g_object_set_data()/get_data()
  so data flow is explicit and types don’t bleed across modules.
]]
#-----------------------------------------------------------------------------
#  END OF FILE