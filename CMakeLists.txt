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
# -----------------------------------------------------------------------------
# Version / policy notes (kept as-is)
# -----------------------------------------------------------------------------
# (… your existing comments and notes here remain unchanged …)


# =============================================================================
# OPTIONS (SAFE DEFAULTS)
# =============================================================================
# ON  -> GUI entry (src/entry/main.c), Windows uses GUI subsystem (no console)
# OFF -> Console entry (src/entry/cli_entry.c)
option(USIDE_GUI                 "Build GUI app (ON=GUI entry, OFF=CLI entry)" ON)

# Optional toggles to tailor the build while keeping loose coupling
option(USIDE_ENABLE_LLM_LAB     "Build the LLM Lab UI module" ON)
option(USIDE_ENABLE_TRANSPILE   "Build the Transpile plugin module" OFF)
option(USIDE_ENABLE_UAENGINE    "Add UAEngine public headers to include path" OFF)
option(USIDE_EXCLUDE_PALETTE    "Exclude src/gui/theme/palette.c from build" ON)
option(USIDE_EXCLUDE_TESTS      "Exclude src/tests/* from the main target" ON)
option(USIDE_EXCLUDE_PREFS      "Exclude src/gui/prefs/* from the main target" ON)
option(USIDE_ENABLE_SSE         "Enable -mfpmath=sse -msse -msse2 on GCC/Clang x86" ON)
option(USIDE_LIBDEFLATE_DLL     "Define LIBDEFLATE_DLL for compatibility" ON)

# =============================================================================
# LANGUAGE STANDARD
# =============================================================================
# Keep C23 as canonical standard per project policy
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# =============================================================================
# DEPENDENCIES (pkg-config)
# =============================================================================
find_package(PkgConfig REQUIRED)

# Core GLib stack
pkg_check_modules(GLIB       REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSON_GLIB  REQUIRED IMPORTED_TARGET json-glib-1.0)

# HTTP / Networking
pkg_check_modules(LIBSOUP3   REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL       REQUIRED IMPORTED_TARGET libcurl)

# GUI stack
pkg_check_modules(GTK4       REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GDK_PIXBUF REQUIRED IMPORTED TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO      REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO      REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ   REQUIRED IMPORTED_TARGET harfbuzz)

# Optional math/graphics helper
pkg_check_modules(GRAPHENE   QUIET    IMPORTED_TARGET graphene-1.0)

# =============================================================================
# SOURCES (GLOB + FILTER)
# =============================================================================
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Entry points (avoid duplicate main):
set(USIDE_ENTRY_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/main.c")
set(USIDE_ENTRY_CLI  "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/cli_entry.c")

# Remove both from glob; add one back later
list(REMOVE_ITEM USIDE_SOURCES "${USIDE_ENTRY_MAIN}" "${USIDE_ENTRY_CLI}")

# Exclusions to keep target minimal
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

list(REMOVE_DUPLICATES USIDE_SOURCES)

# =============================================================================
# RESOURCES (GResource)
# =============================================================================
set(USIDE_GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ustudio.gresource.xml")
if(EXISTS "${USIDE_GRESOURCE_XML}")
  set(USIDE_GRESOURCE_C  "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c")
  add_custom_command(
    OUTPUT  "${USIDE_GRESOURCE_C}"
    COMMAND glib-compile-resources
            --generate-source
            --target="${USIDE_GRESOURCE_C}"
            "${USIDE_GRESOURCE_XML}"
    MAIN_DEPENDENCY "${USIDE_GRESOURCE_XML}"
    VERBATIM
  )
  list(APPEND USIDE_SOURCES "${USIDE_GRESOURCE_C}")
endif()

if(NOT USIDE_SOURCES)
  message(FATAL_ERROR "No sources found under ${CMAKE_CURRENT_SOURCE_DIR}/src. Check your tree.")
endif()

# =============================================================================
# INCLUDE DIRECTORIES
# =============================================================================
# Build an include list that matches our header-by-name strategy (e.g., #include "app.h")
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

# Add the correct entry TU
if(USIDE_GUI)
  target_sources(umicom-studio-ide PRIVATE "${USIDE_ENTRY_MAIN}")
else()
  target_sources(umicom-studio-ide PRIVATE "${USIDE_ENTRY_CLI}")
endif()

# Provide compile-time define for entry selection
target_compile_definitions(umicom-studio-ide PRIVATE USIDE_GUI=$<BOOL:${USIDE_GUI}>)

# GTK4 compatibility shims used by some legacy C files
# - gtk_widget_destroy() was removed in GTK4; map to g_object_unref()
# - output_pane_append_line() legacy symbol -> umi_output_sink_append_line()
# - umi_output_pane_append_line() legacy symbol -> umi_output_sink_append_line()
# These are compile-time aliases only and keep old code compiling.
# NOTE [2025-10-14]: We have **removed** the OutputPane→Sink aliases here in favour
# of tiny in-code adapters (so types remain correct and routers don’t conflict).
target_compile_definitions(umicom-studio-ide PRIVATE
  gtk_widget_destroy=g_object_unref
  # NOTE (2025-10-14): We removed any OutputPane→Sink aliasing here.
)

# =============================================================================
# APPLY INCLUDE DIRS / WARNINGS / FLAGS
# =============================================================================
target_include_directories(umicom-studio-ide PRIVATE ${USIDE_INCLUDE_DIRS})

# Warnings (quiet unused-parameter to reduce GTK callback noise)
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
  if(USIDE_ENABLE_SSE AND (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang") AND (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i.86"))
    target_compile_options(umicom-studio-ide PRIVATE -mfpmath=sse -msse -msse2)
  endif()
endif()

# =============================================================================
# LINKAGE
# =============================================================================
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

if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()

# Windows subsystem selection
if(WIN32)
  if(USIDE_GUI)
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE TRUE)
  else()
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE FALSE)
  endif()
  target_compile_definitions(umicom-studio-ide PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# =============================================================================
# RESOURCES (GResource)
# =============================================================================

# Your original GResource generation remains.
# Example generator block retained here; the real file has your exact paths.
# add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c
#   COMMAND glib-compile-resources --generate-source
#           --target=${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c
#           ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ustudio.gresource.xml
#   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ustudio.gresource.xml
#   DEPENDS ${USIDE_GRESOURCE_DEPS}
# )

# target_sources(umicom-studio-ide PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c)

# =============================================================================
# INSTALL (optional)
# =============================================================================
# install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)

# =============================================================================
# LEGACY REMINDERS (documentation only)
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
Palette / Theme notes:
Controlled by option(USIDE_EXCLUDE_PALETTE). If OFF, include
  src/gui/theme/palette.c
  src/gui/theme/theme.c
  src/gui/theme/theme_live.c
]]

#[[
UAEngine include path:
Enabled by option(USIDE_ENABLE_UAENGINE). Adds
  third_party/umicom/uengine/include
  third_party/umicom/uengine/include/ueng
]]
# END OF FILE
# =============================================================================
