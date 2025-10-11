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

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(USIDE_GUI "Build GUI subsystem on Windows (no console window)" ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Default to a reasonable build type if none supplied
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

# ---------------------------------------------------------------------------
# Dependencies (pkg-config)
# ---------------------------------------------------------------------------
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

# ---------------------------------------------------------------------------
# Sources: pick up every .c file under src/**
# ---------------------------------------------------------------------------
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# ---------------------------------------------------------------------------
# Headers: add every */include under src/**
# This matches your “each module has its own include/” convention.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE USIDE_INCLUDE_DIRS
    LIST_DIRECTORIES TRUE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/include"
)

# Third-party public headers you actually include from our code:
# (adjust if you relocate or trim third_party later)
list(APPEND USIDE_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/umicom/uengine/include/ueng"
)

# De-duplicate include dirs
list(REMOVE_DUPLICATES USIDE_INCLUDE_DIRS)

# ---------------------------------------------------------------------------
# Resources (GLib/GResource)
# Generates C sources from assets/data/ustudio.gresource.xml
# ---------------------------------------------------------------------------
set(USIDE_GRESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/assets/data/ustudio.gresource.xml")
if(EXISTS "${USIDE_GRESOURCE_XML}")
  set(USIDE_GRESOURCE_C   "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.c")
  set(USIDE_GRESOURCE_H   "${CMAKE_CURRENT_BINARY_DIR}/ustudio_gresource.h")
  add_custom_command(
    OUTPUT "${USIDE_GRESOURCE_C}" "${USIDE_GRESOURCE_H}"
    COMMAND glib-compile-resources
            --generate-source
            --target="${USIDE_GRESOURCE_C}"
            --sourcedir="${CMAKE_CURRENT_SOURCE_DIR}/assets/data"
            "${USIDE_GRESOURCE_XML}"
    DEPENDS "${USIDE_GRESOURCE_XML}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/data"
    VERBATIM
  )
  list(APPEND USIDE_SOURCES "${USIDE_GRESOURCE_C}")
else()
  message(WARNING "GResource XML not found at ${USIDE_GRESOURCE_XML} — UI may be incomplete.")
endif()

# ---------------------------------------------------------------------------
# Target
# ---------------------------------------------------------------------------
add_executable(umicom-studio-ide ${USIDE_SOURCES})

# Windows subsystem switch
if(WIN32 AND USIDE_GUI)
  # GUI subsystem (no console window)
  set_target_properties(umicom-studio-ide PROPERTIES
    WIN32_EXECUTABLE YES
  )
endif()

# Include paths
target_include_directories(umicom-studio-ide
  PRIVATE
    ${USIDE_INCLUDE_DIRS}
)

# Compile defs/flags
if(WIN32)
  target_compile_definitions(umicom-studio-ide PRIVATE
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
  )
endif()

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

# Stricter warnings are helpful while we stabilize includes
if(MSVC)
  target_compile_options(umicom-studio-ide PRIVATE /W4)
else()
  target_compile_options(umicom-studio-ide PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Install (optional, simple layout)
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS umicom-studio-ide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


# --- Auto-discovered include dirs (do not remove; generated by assistant) ---
include_directories(
    ${CMAKE_SOURCE_DIR}/src/build/include
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/editor/include
    ${CMAKE_SOURCE_DIR}/src/gui/app/include
    ${CMAKE_SOURCE_DIR}/src/gui/chrome/include
    ${CMAKE_SOURCE_DIR}/src/gui/dialogs/include
    ${CMAKE_SOURCE_DIR}/src/gui/llm_lab/include
    ${CMAKE_SOURCE_DIR}/src/gui/menu/include
    ${CMAKE_SOURCE_DIR}/src/gui/prefs/include
    ${CMAKE_SOURCE_DIR}/src/gui/theme/include
    ${CMAKE_SOURCE_DIR}/src/panes/chat/include
    ${CMAKE_SOURCE_DIR}/src/panes/output/include
    ${CMAKE_SOURCE_DIR}/src/panes/problems/include
    ${CMAKE_SOURCE_DIR}/src/search/include
    ${CMAKE_SOURCE_DIR}/src/ui/include
    ${CMAKE_SOURCE_DIR}/src/util/fs/include
    ${CMAKE_SOURCE_DIR}/src/util/git/include
    ${CMAKE_SOURCE_DIR}/src/util/log/include
    ${CMAKE_SOURCE_DIR}/src/util/platform/include
    ${CMAKE_SOURCE_DIR}/src/util/sys/include
    ${CMAKE_SOURCE_DIR}/src/util/watchers/include
)
