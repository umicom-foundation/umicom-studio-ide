# -----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Clean, reproducible build for MSYS2/MinGW (Windows) and Linux.
#          Fixes prior issues where include directories were incorrectly
#          added as -D macros (producing errors like 'macro names must be
#          identifiers'). Also selects Console vs GUI subsystem sanely.
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-09 | MIT
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)               # Require modern CMake features.

# Project
project(umicom_studio_ide LANGUAGES C)             # Project name + C-only language.

# ---- Build options -----------------------------------------------------------
# ON  -> GUI entry (src/entry/main.c), Windows uses GUI subsystem (no console)
# OFF -> Console entry (src/entry/cli_entry.c)
option(USIDE_GUI "Build GUI app (ON=GUI entry, OFF=console entry)" ON)  # Default to GUI.

set(CMAKE_C_STANDARD 11)                            # Use C11.
set(CMAKE_C_STANDARD_REQUIRED ON)                   # Enforce C11.
set(CMAKE_C_EXTENSIONS OFF)                         # No compiler extensions (strict C11).

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)               # Emit compile_commands.json for tools.

if(NOT CMAKE_BUILD_TYPE)                            # Provide a sensible default build type.
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

# ---- Dependencies (pkg-config via imported targets) --------------------------
find_package(PkgConfig REQUIRED)                    # We use pkg-config hints/targets.

# Core GLib/GIO stack needed across the codebase:
pkg_check_modules(GLIB      REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT   REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO       REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(JSON_GLIB REQUIRED IMPORTED_TARGET json-glib-1.0)

# Networking pieces used by LLM/HTTP layers:
pkg_check_modules(LIBSOUP3  REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL      REQUIRED IMPORTED_TARGET libcurl)

# GUI stack (we build GUI by default; keep GTK4 + friends available):
pkg_check_modules(GTK4      REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO     REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO     REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ  REQUIRED IMPORTED_TARGET harfbuzz)

# Optional math/graphics helper if present (won't fail if missing):
pkg_check_modules(GRAPHENE  QUIET    IMPORTED_TARGET graphene-1.0)

# ---- Sources ----------------------------------------------------------------
# Grab every .c under src/**
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# We have two possible entry points: GUI and CLI. Avoid duplicate 'main'.
set(USIDE_ENTRY_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/main.c")       # GUI entry.
set(USIDE_ENTRY_CLI  "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/cli_entry.c")  # Console entry.

# Remove both entries from the glob list first; we will add exactly one back.
list(REMOVE_ITEM USIDE_SOURCES "${USIDE_ENTRY_MAIN}" "${USIDE_ENTRY_CLI}")

# Choose the single entry that matches the option.
if(USIDE_GUI)
  if(EXISTS "${USIDE_ENTRY_MAIN}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_MAIN}")     # Build GUI entry.
  else()
    message(FATAL_ERROR "USIDE_GUI=ON but not found: ${USIDE_ENTRY_MAIN}")
  endif()
else()
  if(EXISTS "${USIDE_ENTRY_CLI}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_CLI}")      # Build console entry.
  else()
    message(FATAL_ERROR "USIDE_GUI=OFF but not found: ${USIDE_ENTRY_CLI}")
  endif()
endif()

# ---- Include directories -----------------------------------------------------
# Only add public headers that live under src/**/include (keeps include path clean).
file(GLOB_RECURSE USIDE_INCLUDE_DIRS
  LIST_DIRECTORIES TRUE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include"
)

# Third-party public headers (uengine, etc.)
list(APPEND USIDE_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
)

list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)          # Deduplicate include paths.

# ---- IMPORTANT: No XML/GResource ------------------------------------------------
# We intentionally DO NOT call glib-compile-resources or reference any .xml/.ui.
# Pure C resources (e.g., src/ui/resources/ustudio_resources.c) are compiled as normal sources.

# ---- Target -----------------------------------------------------------------
add_executable(umicom-studio-ide ${USIDE_SOURCES})  # Create the final executable.

# Tell the code whether we are GUI or not (usable in #if USIDE_GUI blocks).
target_compile_definitions(umicom-studio-ide PRIVATE USIDE_GUI=$<BOOL:${USIDE_GUI}>)

# Windows: when building GUI, switch to GUI subsystem (no console window).
if(WIN32 AND USIDE_GUI)
  set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE YES)
endif()


set(USIDE_THEME_PALETTE "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme/palette.c")
if(EXISTS "${USIDE_THEME_PALETTE}")
  list(REMOVE_ITEM USIDE_SOURCES "${USIDE_THEME_PALETTE}")
endif()


# Add include paths (also add binary dir in case we generate headers in future).
target_include_directories(umicom-studio-ide PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
   ${USIDE_INCLUDE_DIRS}
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# Common warnings (quiet unused-parameter to reduce GTK callback noise).
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Link libraries from pkg-config imported targets.
target_link_libraries(umicom-studio-ide PRIVATE
  PkgConfig::GTK4
  PkgConfig::GLIB
  PkgConfig::GOBJECT
  PkgConfig::GIO
  PkgConfig::JSON_GLIB
  PkgConfig::LIBSOUP3
  PkgConfig::CURL
  PkgConfig::GDK_PIXBUF
  PkgConfig::CAIRO
  PkgConfig::PANGO
  PkgConfig::HARFBUZZ
)

# Link optional Graphene if found (harmless no-op if not found):
if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()

# Windows-specific small hygiene defs:
if(WIN32)
  target_compile_definitions(umicom-studio-ide PRIVATE
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
  )
endif()

# Install rule (optional but handy for `cmake --install`).
include(GNUInstallDirs)
install(TARGETS umicom-studio-ide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# -----------------------------------------------------------------------------
# End of CMakeLists.txt