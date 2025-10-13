# -----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Clean, reproducible build for MSYS2/MinGW (Windows) and Linux.
#          Fixes prior issues where include directories were incorrectly
#          added as -D macros (producing errors like 'macro names must be
#          identifiers'). Also selects Console vs GUI subsystem sanely.
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-09 | MIT
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

# =============================================================================
# PROJECT
# =============================================================================
project(umicom_studio_ide LANGUAGES C)

# =============================================================================
# OPTIONS (SAFE DEFAULTS)
# =============================================================================
# ON  -> GUI entry (src/entry/main.c), Windows uses GUI subsystem (no console)
# OFF -> Console entry (src/entry/cli_entry.c)
option(USIDE_GUI                 "Build GUI app (ON=GUI entry, OFF=console entry)" ON)

# Keep C11 by default until codebase is fully modernized; flip to C23 when ready
option(USIDE_USE_C23            "Compile with C23 instead of C11" OFF)

# Optional modules to keep target loosely-coupled
option(USIDE_ENABLE_LLM_LAB     "Build the LLM Lab UI module" ON)
option(USIDE_ENABLE_TRANSPILE   "Build the Transpile plugin module" OFF)
option(USIDE_ENABLE_UAENGINE    "Add UAEngine public headers to include path" OFF)

# Historically palette.c was excluded; expose as a toggle (default ON to mimic prior state)
option(USIDE_EXCLUDE_PALETTE    "Exclude src/gui/theme/palette.c from build" ON)

# =============================================================================
# LANGUAGE STANDARD & GLOBAL SETTINGS
# =============================================================================
if(USIDE_USE_C23)
  set(CMAKE_C_STANDARD 23)
else()
  set(CMAKE_C_STANDARD 11)
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export for tooling (clangd, static analyzers)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sensible default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

# =============================================================================
# DEPENDENCIES (pkg-config imported targets)
# =============================================================================
find_package(PkgConfig REQUIRED)

# Core GLib/GIO stack
pkg_check_modules(GLIB       REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT    REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO        REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(JSON_GLIB  REQUIRED IMPORTED_TARGET json-glib-1.0)

# Networking / HTTP
pkg_check_modules(LIBSOUP3   REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL       REQUIRED IMPORTED_TARGET libcurl)

# GUI stack
pkg_check_modules(GTK4       REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO      REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO      REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ   REQUIRED IMPORTED_TARGET harfbuzz)

# Optional math/graphics helper (quiet)
pkg_check_modules(GRAPHENE   QUIET    IMPORTED_TARGET graphene-1.0)

# =============================================================================
# SOURCES (GLOB + FILTER TO AVOID DUPLICATE MAINS & UNWANTED MODULES)
# =============================================================================
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Entry points — avoid duplicate 'main'
set(USIDE_ENTRY_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/main.c")
set(USIDE_ENTRY_CLI  "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/cli_entry.c")

# Remove both entries from the glob list; then add exactly one back
list(REMOVE_ITEM USIDE_SOURCES "${USIDE_ENTRY_MAIN}" "${USIDE_ENTRY_CLI}")

# Choose the single entry that matches the option.
if(USIDE_GUI)
  if(EXISTS "${USIDE_ENTRY_MAIN}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_MAIN}")
  else()
    message(FATAL_ERROR "USIDE_GUI=ON but not found: ${USIDE_ENTRY_MAIN}")
  endif()
else()
  if(EXISTS "${USIDE_ENTRY_CLI}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_CLI}")
  else()
    message(FATAL_ERROR "USIDE_GUI=OFF but not found: ${USIDE_ENTRY_CLI}")
  endif()
endif()

# Optional modules — exclude whole folders when OFF (no dead code compiled)
if(NOT USIDE_ENABLE_LLM_LAB)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX "/src/gui/llm_lab/.*\.c$")
endif()
if(NOT USIDE_ENABLE_TRANSPILE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX "/src/plugins/transpile/.*\.c$")
endif()

# Legacy palette exclusion via toggle (default ON)
if(USIDE_EXCLUDE_PALETTE)
  list(FILTER USIDE_SOURCES EXCLUDE REGEX "/src/gui/theme/palette\.c$")
endif()

# =============================================================================
# INCLUDE DIRECTORIES (PUBLIC HEADERS ONLY)
# =============================================================================
# Collect every module's public include dir (src/*/include). This ensures
# headers are included by NAME only (e.g., "umi_output_sink.h"), never as deep
# relative paths like "src/foo/include/bar.h".
file(GLOB_RECURSE USIDE_INCLUDE_DIRS LIST_DIRECTORIES TRUE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include"
)

# Ensure canonical project-wide public headers are visible explicitly
list(APPEND USIDE_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# UAEngine headers are connected but should be optional to avoid accidental
# cross-coupling. Gate behind USIDE_ENABLE_UAENGINE.
if(USIDE_ENABLE_UAENGINE)
  list(APPEND USIDE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
  )
endif()

list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)

# =============================================================================
# TARGET
# =============================================================================
add_executable(umicom-studio-ide ${USIDE_SOURCES})

# Provide compile-time define for entry selection
# (usable in C via `#if USIDE_GUI`)
target_compile_definitions(umicom-studio-ide PRIVATE USIDE_GUI=$<BOOL:${USIDE_GUI}>)

# Windows: use GUI subsystem when USIDE_GUI=ON
enable_language(C)
if(WIN32 AND USIDE_GUI)
  set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE YES)
  # For MinGW, WIN32_EXECUTABLE is typically enough; add -mwindows if required:
  # target_link_options(umicom-studio-ide PRIVATE -mwindows)
endif()

# Include paths for this target (PUBLIC headers only)
target_include_directories(umicom-studio-ide PRIVATE ${USIDE_INCLUDE_DIRS})

# Warnings (quiet unused-parameter to reduce GTK callback noise)
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Link libraries via pkg-config imported targets
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

# Link optional Graphene if found
if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()

# Windows hygiene macros
if(WIN32)
  target_compile_definitions(umicom-studio-ide PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# Dev-time hardening (Debug only) — GCC/Clang; MSVC ignores these
if(CMAKE_BUILD_TYPE MATCHES "Debug" AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
  target_compile_options(umicom-studio-ide PRIVATE -fstack-protector-strong -fno-omit-frame-pointer)
  target_link_options(umicom-studio-ide PRIVATE -fstack-protector-strong)
  # Optional sanitizers (enable if your environment supports them):
  # target_compile_options(umicom-studio-ide PRIVATE -fsanitize=address,undefined)
  # target_link_options(umicom-studio-ide PRIVATE -fsanitize=address,undefined)
endif()

# Install rule (optional)
include(GNUInstallDirs)
install(TARGETS umicom-studio-ide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# =============================================================================
# PRE-EXISTING / LEGACY LINES TO CLEAN UP (wrapped per your instruction)
# =============================================================================
#[[
PLEASE REMOVE (do not remove yourself):
The following shows the WRONG pattern that previously injected include
*directories* via add_definitions(-I...). This is invalid and causes errors
like 'macro names must be identifiers'. Keep this block only as a reminder.

# add_definitions(-IC:/Dev/umicom-studio-ide/src/include)
# add_definitions(-IC:/Dev/umicom-studio-ide/src/build/include)
# add_definitions(-I${CMAKE_CURRENT_SOURCE_DIR}/src/core/include)

Use target_include_directories() instead (already configured above).
]]

#[[
PLEASE REMOVE (do not remove yourself): legacy hard-coded exclusion of palette.c
set(USIDE_THEME_PALETTE "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme/palette.c")
if(EXISTS "${USIDE_THEME_PALETTE}")
  list(REMOVE_ITEM USIDE_SOURCES "${USIDE_THEME_PALETTE}")
endif()
-- Replaced by option(USIDE_EXCLUDE_PALETTE ...) above.
]]

#[[
PLEASE REMOVE (do not remove yourself): unguarded UAEngine include paths
list(APPEND USIDE_INCLUDE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
)
-- Replaced by option(USIDE_ENABLE_UAENGINE ...) above.
]]

# END OF FILE