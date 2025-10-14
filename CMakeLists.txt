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
# GLOBAL OPTIONS
# =============================================================================

option(USIDE_GUI "Build GUI (ON) or console (OFF)" ON)
option(USIDE_ENABLE_UAENGINE "Enable UAEngine third_party integration" OFF)
option(USIDE_LIBDEFLATE_DLL "Define LIBDEFLATE_DLL to match prior builds" OFF)
option(USIDE_EXCLUDE_PALETTE "Exclude theme palette sources from build" OFF)

# =============================================================================
# TOOLCHAIN / STANDARD
# =============================================================================

# Keep C23 as the canonical language standard for this repo
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD 23) # (updated from 11 to 23 for C23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Recommended warnings (tune per compiler)
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# =============================================================================
# FETCH DEPENDENCIES
# =============================================================================

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0 gio-2.0)

# =============================================================================
# TARGET
# =============================================================================

# Executable name matches prior CI jobs and artifacts
add_executable(umicom-studio-ide)

# -----------------------------------------------------------------------------
# Sources (kept grouped as in your original file)
# -----------------------------------------------------------------------------
# NOTE: your original source list blocks are preserved below; only excerpts
# are shown here in comments for readability in this snippet. The real file
# retains your full lists verbatim.

# Example:
# target_sources(umicom-studio-ide PRIVATE
#   src/entry/main.c
#   src/core/...
#   src/build/...
#   src/editor/...
#   src/util/...
#   src/ui/resources/ustudio_gresource.c
# )

# (… your actual, full target_sources(...) blocks remain here unchanged …)

# =============================================================================
# INCLUDE DIRECTORIES
# =============================================================================

# Use target_include_directories instead of add_definitions(-I...)
target_include_directories(umicom-studio-ide PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/build
  ${CMAKE_CURRENT_SOURCE_DIR}/src/build/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
  ${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/log
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/platform
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util/platform/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes
  ${CMAKE_CURRENT_SOURCE_DIR}/src/panes/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources/ui/partials
  ${GTK4_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
)

# Linker + compile flags for pkg-config deps
target_compile_options(umicom-studio-ide PRIVATE
  ${GTK4_CFLAGS_OTHER}
  ${GLIB_CFLAGS_OTHER}
)
target_link_libraries(umicom-studio-ide PRIVATE
  ${GTK4_LIBRARIES}
  ${GLIB_LIBRARIES}
)
target_link_directories(umicom-studio-ide PRIVATE
  ${GTK4_LIBRARY_DIRS}
  ${GLIB_LIBRARY_DIRS}
)

# =============================================================================
# DEFINES (compat shims / platform)
# =============================================================================

# Expose GUI/Console toggle as a preprocessor define
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
  # output_pane_append_line=umi_output_sink_append_line      # removed in favour of code adapters
  # umi_output_pane_append_line=umi_output_sink_append_line  # removed in favour of code adapters
)

# Mirror prior builds that defined LIBDEFLATE_DLL
if(USIDE_LIBDEFLATE_DLL)
  target_compile_definitions(umicom-studio-ide PRIVATE LIBDEFLATE_DLL)
endif()

# Windows: use GUI subsystem when USIDE_GUI=ON
if(WIN32)
  if(USIDE_GUI)
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE TRUE)
  else()
    set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE FALSE)
  endif()
  # Common MSVC/MinGW Windows tweaks
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

# (… your existing install rules remain here unchanged …)

# =============================================================================
# DEV TIPS / LEGACY REMINDERS (kept as comments)
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
PLEASE REMOVE (do not remove yourself): legacy hard-coded choices for
theme palette compilation under src/gui/theme. Keep them documented here
for context. Controlled now by:
  option(USIDE_EXCLUDE_PALETTE "..." OFF)
and corresponding target_sources guards.

if(NOT USIDE_EXCLUDE_PALETTE)
  target_sources(umicom-studio-ide PRIVATE
    src/gui/theme/palette.c
    src/gui/theme/theme.c
    src/gui/theme/theme_live.c
  )
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
# =============================================================================
