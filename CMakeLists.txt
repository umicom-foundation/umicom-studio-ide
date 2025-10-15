# -----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
#
# PURPOSE (read me first):
#   This is the single script that teaches CMake how to build the project.
#   Think of it like a recipe: we tell CMake which sources exist, which
#   libraries are needed (GTK, GLib, etc.), which compiler options to use,
#   and how to produce the final program (the .exe on Windows).
#
#   A PRIOR BUG WE FIX HERE:
#   ------------------------
#   The project previously called:
#       target_include_directories(umicom-studio-ide …)
#       target_link_libraries(umicom-studio-ide …)
#   *before* creating the target with add_executable().
#   In CMake, you can only attach properties to a target AFTER the target
#   exists. Doing it earlier yields the error you saw:
#     "Cannot specify include directories for target 'umicom-studio-ide'
#      which is not built by this project."
#   The fixed file below creates the executable first, then sets include
#   paths and libraries on that executable.
#
# WHO IS THIS FILE FOR?
#   New contributors and beginners to C + CMake. All sections are documented
#   with what/why, so you can learn just by reading. If you’re totally new,
#   skim from top to bottom once; it will make day-to-day changes much easier.
#
# PLATFORM TARGETS:
#   - Windows (MSYS2/MinGW64): uses PkgConfig to find GTK and friends.
#   - Linux: same idea; as long as pkg-config and the dev packages exist,
#            this file will work unchanged.
#
# LICENSE:
#   MIT – see repository license. Comments are extensive so new folks can
#   learn; please keep them generous when you edit things.
#
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-09 | MIT
# -----------------------------------------------------------------------------

# Tell CMake the minimum version we rely on (features we use require 3.20+).
cmake_minimum_required(VERSION 3.20)

# Define the project name and the languages we compile. We only use C.
project(umicom_studio_ide LANGUAGES C)

# ------------------------------ Versioning -----------------------------------
# We keep a simple 3-part semantic version here. Not strictly required for
# building, but useful if later you want to bake the version into the binary.
set(UMICOM_STUDIO_IDE_VERSION_MAJOR 0)
set(UMICOM_STUDIO_IDE_VERSION_MINOR 9)
set(UMICOM_STUDIO_IDE_VERSION_PATCH 0)
set(UMICOM_STUDIO_IDE_VERSION
  "${UMICOM_STUDIO_IDE_VERSION_MAJOR}.${UMICOM_STUDIO_IDE_VERSION_MINOR}.${UMICOM_STUDIO_IDE_VERSION_PATCH}"
)

# ------------------------------ Build Options --------------------------------
# Options are ON/OFF toggles a user can pass at configure time, e.g.:
#   cmake -S . -B build -DUSIDE_GUI=ON
# If you don’t pass anything, the default after the label is used.

# Build as a GUI app (no console on Windows) vs a console app (with console).
# We always produce ONE executable; the subsystem toggles how Windows launches.
option(USIDE_GUI                 "Build GUI app (ON=GUI subsystem, OFF=console)" ON)

# Feature flags for optional parts of the source tree. These let newcomers
# trim the build while iterating or experimenting.
option(USIDE_ENABLE_LLM_LAB     "Build the LLM Lab UI module" ON)
option(USIDE_ENABLE_TRANSPILE   "Build the Transpile plugin module" OFF)
option(USIDE_ENABLE_UAENGINE    "Add UAEngine public headers to include path" OFF)

# Convenience excludes so beginners don’t compile extra modules yet.
option(USIDE_EXCLUDE_PALETTE    "Exclude src/gui/theme/palette.c from build" ON)
option(USIDE_EXCLUDE_TESTS      "Exclude src/tests/* from the main target" ON)
option(USIDE_EXCLUDE_PREFS      "Exclude src/gui/prefs/* from the main target" ON)

# Performance/compat switches for x86 compilers (harmless ON by default).
option(USIDE_ENABLE_SSE         "Enable -mfpmath=sse -msse -msse2 on GCC/Clang x86" ON)

# Some third-party libs (e.g., libdeflate) prefer this macro on Windows DLLs.
option(USIDE_LIBDEFLATE_DLL     "Define LIBDEFLATE_DLL for compatibility" ON)

# ------------------------------ Language Std ---------------------------------
# We use modern C (C23 here). If your compiler is a bit older you can try 17.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ------------------------------- Dependencies --------------------------------
# We depend on GUI/toolkit libs that are discoverable via pkg-config.
# CMake’s PkgConfig module allows us to ask for those packages and gives us
# imported targets (PkgConfig::GTK4 etc.) we can link to cleanly.
find_package(PkgConfig REQUIRED)

# The IMPORTED_TARGET keyword asks for "PkgConfig::<name>" targets we can link.
# If a module is REQUIRED and missing, CMake stops with a clear message.
pkg_check_modules(GLIB       REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSON_GLIB  REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(LIBSOUP3   REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL       REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(GTK4       REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO      REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO      REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ   REQUIRED IMPORTED_TARGET harfbuzz)
# GRAPHENE is nice to have, but not mandatory on all setups.
pkg_check_modules(GRAPHENE   QUIET    IMPORTED_TARGET graphene-1.0)
# GtkSourceView provides a better code editor widget. We keep it optional so
# the app still builds/runs without it.
pkg_check_modules(GSV        QUIET    IMPORTED_TARGET gtksourceview-5)

# ------------------------------- Source Files --------------------------------
# We gather all .c files under src/ recursively. CONFIGURE_DEPENDS asks CMake
# to re-scan if new files are added (useful during development).
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Beginners: the filters below just remove certain folders from the list of
# sources so your first build stays small and fast.
if(USIDE_EXCLUDE_TESTS)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/tests/.*\\.c$")
endif()
if(USIDE_EXCLUDE_PREFS)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/prefs/.*\\.c$")
endif()
if(USIDE_EXCLUDE_PALETTE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/theme/palette\\.c$")
endif()
if(NOT USIDE_ENABLE_LLM_LAB)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gui/llm_lab/.*\\.c$")
endif()
if(NOT USIDE_ENABLE_TRANSPILE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/plugins/transpile/.*\\.c$")
endif()

# Avoid duplicates in case patterns overlapped.
list(REMOVE_DUPLICATES USIDE_SOURCES)

# ------------------------------ GTK Resources --------------------------------
# GTK can bundle images/UI files via GResource. If your XML description is
# present, we generate a .c file from it and add it to the sources.
set(USIDE_GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ustudio.gresource.xml")
if(EXISTS "${USIDE_GRESOURCE_XML}")
  set(USIDE_GRESOURCE_C  "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c")
  add_custom_command(
    OUTPUT  "${USIDE_GRESOURCE_C}"                # the .c file we will compile
    COMMAND glib-compile-resources                # tool that does the work
            --generate-source                     # ask for C source output
            --target="${USIDE_GRESOURCE_C}"       # where to write it
            "${USIDE_GRESOURCE_XML}"              # the XML manifest file
    MAIN_DEPENDENCY "${USIDE_GRESOURCE_XML}"      # rebuild if XML changes
    VERBATIM                                      # no shell interpretation
  )
  list(APPEND USIDE_SOURCES "${USIDE_GRESOURCE_C}")  # now part of the build
endif()

# If we somehow filtered everything away, help the developer quickly.
if(NOT USIDE_SOURCES)
  message(FATAL_ERROR "No sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src. Check your tree.")
endif()

# --------------------------- Project Include Paths ---------------------------
# These are the *project* folders we want the compiler to search when a source
# file does #include "something.h". We keep it explicit and readable so new
# folks can discover where headers live.
set(USIDE_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/build/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/platform"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/util/platform/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ui"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ui/partials"
)

# Also pick up any directory named “…/include” anywhere under src/.
file(GLOB_RECURSE USIDE_AUTO_INCLUDE_DIRS LIST_DIRECTORIES true
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include")
list(APPEND USIDE_INCLUDE_DIRS ${USIDE_AUTO_INCLUDE_DIRS})

# If a contributor enables UAEngine headers, extend the search path.
if(USIDE_ENABLE_UAENGINE)
  list(APPEND USIDE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
  )
endif()

# Clean the list to avoid duplicates.
list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)

# ============================== THE TARGET (IMPORTANT ORDER) =================
# We NOW create the executable target. From this point onward, we are allowed
# to attach include paths, compile options and link libraries to it.
add_executable(umicom-studio-ide ${USIDE_SOURCES})

# Attach our project include paths to the target so all sources can #include
# local headers without long relative paths.
target_include_directories(umicom-studio-ide PRIVATE ${USIDE_INCLUDE_DIRS})

# Provide small, project-wide compile-time defines:
#  - USIDE_GUI is visible to code (0 or 1) so code can tweak behavior.
#  - gtk_widget_destroy alias avoids GTK deprecation warnings when we destroy
#    with g_object_unref() in some code paths.
target_compile_definitions(umicom-studio-ide PRIVATE
  USIDE_GUI=$<BOOL:${USIDE_GUI}>
  gtk_widget_destroy=g_object_unref
)

# If a third-party library expects LIBDEFLATE_DLL defined on Windows, set it.
if(USIDE_LIBDEFLATE_DLL)
  target_compile_definitions(umicom-studio-ide PRIVATE LIBDEFLATE_DLL)
endif()

# ------------------------------- Warnings & Flags ----------------------------
# New coders benefit from strong warnings; we enable them across compilers.
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE
    -Wall -Wextra -Wpedantic           # strong warnings
    -Wno-unused-parameter              # but don’t punish placeholder args
  )
  # Optional SSE flags (safe no-ops on non-x86).
  if(USIDE_ENABLE_SSE AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
     AND (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i.86"))
    target_compile_options(umicom-studio-ide PRIVATE -mfpmath=sse -msse -msse2)
  endif()
endif()

# --------------------------------- Linking -----------------------------------
# Link against the pkg-config IMPORTED targets we discovered above. These
# targets “carry” their correct include/lib paths for the platform.
target_link_libraries(umicom-studio-ide PRIVATE
  PkgConfig::GLIB
  PkgConfig::JSON_GLIB
  PkgConfig::LIBSOUP3
  PkgConfig::CURL
  PkgConfig::GTK4
  PkgConfig::GDK_PIXBUF
  PkgConfig::CAIRO
  PkgConfig::PANGO
  PkgConfig::HARFBUZZ
)

# Only link GRAPHENE if pkg-config found it (it’s optional).
if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()

# Link GtkSourceView if available (optional, the app still runs without it).
if(TARGET PkgConfig::GSV)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GSV)
endif()

# -------------------------- Windows-specific bits ----------------------------
if(WIN32)
  # Choose GUI vs Console subsystem. For MSVC this boolean is enough; for
  # MinGW we also add an explicit linker flag just below.
  if(USIDE_GUI)
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE TRUE)
  else()
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE FALSE)
  endif()

  # MinGW startup subsystem: this ensures the right entry point is selected.
  # If you ever see “undefined reference to WinMain”, you probably linked the
  # console build with the windows subsystem (or vice versa).
  if(MINGW)
    target_link_options(umicom-studio-ide PRIVATE
      $<$<BOOL:${USIDE_GUI}>:-Wl,--subsystem,windows>
      $<$<NOT:$<BOOL:${USIDE_GUI}>>:-Wl,--subsystem,console>
    )
  endif()

  # Small Windows quality-of-life defines: avoid insecure CRT warnings and
  # keep <windows.h> from defining min/max macros that break std::min/max, etc.
  target_compile_definitions(umicom-studio-ide PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# ---------------------------------- Install ----------------------------------
# Optional: if you later “make install”, the executable will be copied to bin/.
# (We keep it commented for now to keep the first steps simple.)
# install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)

# ------------------------------- Documentation -------------------------------
#[[
IMPORTANT “bad example” (kept here as a learning aid, DO NOT UNCOMMENT):
  add_definitions(-IC:/some/include/path)
This tries to inject an *include directory* using add_definitions(), which
is meant for preprocessor macros, not include paths. It leads to cryptic
errors like “macro names must be identifiers”. The correct way is what we
use above:
  target_include_directories(umicom-studio-ide PRIVATE <paths...>)

Palette / Theme notes:
  The file src/gui/theme/palette.c is excluded by default to keep the first
  build small. Flip -DUSIDE_EXCLUDE_PALETTE=OFF to include it.

UAEngine include path:
  If you enable -DUSIDE_ENABLE_UAENGINE=ON we add third_party/umicom/uengine
  headers to the include path (only headers; linking remains up to the plugin).
]]
# -------------------------------- End of file --------------------------------
