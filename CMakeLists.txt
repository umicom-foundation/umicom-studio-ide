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

# Project
project(umicom_studio_ide LANGUAGES C)

# -----------------------------------------------------------------------------
# Build options
# -----------------------------------------------------------------------------
# ON  -> GUI entry (src/entry/main.c), Windows uses GUI subsystem (no console)
# OFF -> Console entry (src/entry/cli_entry.c)
option(USIDE_GUI "Build GUI entry (main.c). OFF picks console entry (cli_entry.c)." ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Export compile_commands.json (helps IDEs, clangd, static analyzers)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to a reasonable build type if none supplied
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

# -----------------------------------------------------------------------------
# Dependencies (pkg-config)
# -----------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

# Core GTK/GLib stack
pkg_check_modules(GTK4          REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GLIB          REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT       REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO           REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(JSON_GLIB     REQUIRED IMPORTED_TARGET json-glib-1.0)

# Networking / images / text
pkg_check_modules(LIBSOUP3      REQUIRED IMPORTED_TARGET libsoup-3.0)
pkg_check_modules(CURL          REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(GDK_PIXBUF    REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO         REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(PANGO         REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(HARFBUZZ      REQUIRED IMPORTED_TARGET harfbuzz)

# Optional (present on many GTK stacks)
pkg_check_modules(GRAPHENE      QUIET    IMPORTED_TARGET graphene-1.0)

# -----------------------------------------------------------------------------
# Sources
#  - Start with all sources under src/**
#  - Then explicitly choose ONE entry file (GUI or CLI) to avoid duplicate 'main'
# -----------------------------------------------------------------------------
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Absolute paths to the two possible entries
set(USIDE_ENTRY_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/main.c")
set(USIDE_ENTRY_CLI  "${CMAKE_CURRENT_SOURCE_DIR}/src/entry/cli_entry.c")

# Remove both entries from the globbed list first (avoid duplicate main symbols)
list(REMOVE_ITEM USIDE_SOURCES "${USIDE_ENTRY_MAIN}" "${USIDE_ENTRY_CLI}")

# Choose the single entry to compile based on USIDE_GUI, fail loudly if missing
if(USIDE_GUI)
  if(EXISTS "${USIDE_ENTRY_MAIN}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_MAIN}")
  else()
    message(FATAL_ERROR "USIDE_GUI=ON but entry file not found: ${USIDE_ENTRY_MAIN}")
  endif()
else()
  if(EXISTS "${USIDE_ENTRY_CLI}")
    list(APPEND USIDE_SOURCES "${USIDE_ENTRY_CLI}")
  else()
    message(FATAL_ERROR "USIDE_GUI=OFF but entry file not found: ${USIDE_ENTRY_CLI}")
  endif()
endif()

# -----------------------------------------------------------------------------
# Headers: add every */include under src/**
# Matches “each module has its own include/” convention.
# -----------------------------------------------------------------------------
file(GLOB_RECURSE USIDE_INCLUDE_DIRS
    LIST_DIRECTORIES TRUE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include"
)

# Third-party public headers used by this project
list(APPEND USIDE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
)

list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)

# -----------------------------------------------------------------------------
# Resources (GLib/GResource)
#  - Generates a single C source from src/ui/resources/ustudio.gresource.xml
#  - NOTE: Do NOT list a header in OUTPUT unless you actually --generate-header.
# -----------------------------------------------------------------------------
set(USIDE_GRESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources")
set(USIDE_GRESOURCE_XML "${USIDE_GRESOURCE_DIR}/ustudio.gresource.xml")

if(EXISTS "${USIDE_GRESOURCE_XML}")
  # Find the tool so builds fail fast & clearly if it's missing
  find_program(GLIB_COMPILE_RESOURCES glib-compile-resources)
  if(NOT GLIB_COMPILE_RESOURCES)
    message(FATAL_ERROR "Missing tool: glib-compile-resources (part of GLib). Please install glib2 tools.")
  endif()

  set(USIDE_GRESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c")

  add_custom_command(
    OUTPUT "${USIDE_GRESOURCE_C}"
    COMMAND "${GLIB_COMPILE_RESOURCES}"
            --generate-source
            --target="${USIDE_GRESOURCE_C}"
            --sourcedir="${USIDE_GRESOURCE_DIR}"
            "${USIDE_GRESOURCE_XML}"
    DEPENDS "${USIDE_GRESOURCE_XML}"
    WORKING_DIRECTORY "${USIDE_GRESOURCE_DIR}"
    VERBATIM
  )
  list(APPEND USIDE_SOURCES "${USIDE_GRESOURCE_C}")
else()
  message(WARNING "GResource XML not found at ${USIDE_GRESOURCE_XML} — UI may be incomplete.")
endif()

# -----------------------------------------------------------------------------
# Target
# -----------------------------------------------------------------------------
add_executable(umicom-studio-ide ${USIDE_SOURCES})

# Windows subsystem switch (only meaningful on Windows)
if(WIN32 AND USIDE_GUI)
  # GUI subsystem (no console window)
  set_target_properties(umicom-studio-ide PROPERTIES WIN32_EXECUTABLE YES)
endif()

# Include paths (add binary dir too in case generated files are included later)
target_include_directories(umicom-studio-ide PRIVATE
  ${USIDE_INCLUDE_DIRS}
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# Compile defs/flags
if(WIN32)
  target_compile_definitions(umicom-studio-ide PRIVATE
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
  )
endif()

# Stricter warnings; silence unused-parameter noise common with GTK callbacks
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# (Optional) GLib API deprecation windows – adjust to your stack versions
# target_compile_definitions(umicom-studio-ide PRIVATE
#   GLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_70
#   GLIB_VERSION_MAX_ALLOWED=GLIB_VERSION_2_78
# )

# Link libraries from pkg-config
target_link_libraries(umicom-studio-ide
  PRIVATE
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
if(TARGET PkgConfig::GRAPHENE)
  target_link_libraries(umicom-studio-ide PRIVATE PkgConfig::GRAPHENE)
endif()

# Install (optional)
include(GNUInstallDirs)
install(TARGETS umicom-studio-ide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
# -----------------------------------------------------------------------------