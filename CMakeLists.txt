#-----------------------------------------------------------------------------
# Umicom Studio IDE (USIDE)
# File: CMakeLists.txt
# PURPOSE: Build configuration for MinGW/MSYS2 + GTK4. 
#
# Created by: Umicom Foundation (https://umicom.foundation/)
# Author: Sammy Hegab
# Date: 15-09-2025
# License: MIT
#---------------------------------------------------------------------------*/

cmake_minimum_required(VERSION 3.20)
project(umicom_studio_ide C)

# Require C11 and common warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wno-missing-field-initializers)

# --- Dependencies (MSYS2) ---------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(GDKPIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(HARFBUZZ REQUIRED IMPORTED_TARGET harfbuzz)
pkg_check_modules(GRAPHENE REQUIRED IMPORTED_TARGET graphene-1.0)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GOBJECT REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(JSONGLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(SOUP3 REQUIRED IMPORTED_TARGET libsoup-3.0)

# --- Sources -----------------------------------------------------------------
# Grab every .c under src, but ignore accidental duplicate subtree "src/src"
file(GLOB_RECURSE USTUDIO_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.c"
)
# Filter out any path containing '/src/src/' (accidental duplicates)
set(USTUDIO_FILTERED_SOURCES "")
foreach(_f IN LISTS USTUDIO_SOURCES)
  string(FIND "${_f}" "/src/src/" _has_dup)
  if(_has_dup EQUAL -1)
    list(APPEND USTUDIO_FILTERED_SOURCES "${_f}")
  endif()
endforeach()
list(LENGTH USTUDIO_FILTERED_SOURCES NUM_C_SOURCES)
message(STATUS "umicom-studio sources: ${CMAKE_SOURCE_DIR}")
message(STATUS "Number of C sources: ${NUM_C_SOURCES}")

add_executable(ustudio ${USTUDIO_FILTERED_SOURCES})

# --- Include directories (so headers like editor/editor.h resolve) -----------
target_include_directories(ustudio PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/editor"
  "${CMAKE_SOURCE_DIR}/src/gui"
  "${CMAKE_SOURCE_DIR}/src/core"
  "${CMAKE_SOURCE_DIR}/src/panes"
  "${CMAKE_SOURCE_DIR}/src/plugins"
  "${CMAKE_SOURCE_DIR}/src/plugins/transpile"
  "${CMAKE_SOURCE_DIR}/src/build"
  "${CMAKE_SOURCE_DIR}/src/llm"
  "${CMAKE_SOURCE_DIR}/src/llm/providers"
  "${CMAKE_SOURCE_DIR}/src/search"
  "${CMAKE_SOURCE_DIR}/src/ui"
)

# --- Link libraries ----------------------------------------------------------
target_link_libraries(ustudio PRIVATE
  PkgConfig::GTK4
  PkgConfig::PANGO
  PkgConfig::GDKPIXBUF
  PkgConfig::CAIRO
  PkgConfig::HARFBUZZ
  PkgConfig::GRAPHENE
  PkgConfig::GLIB
  PkgConfig::GOBJECT
  PkgConfig::GIO
  PkgConfig::JSONGLIB
  PkgConfig::CURL
  PkgConfig::SOUP3
)

# --- Windows / MinGW launch configuration -----------------------------------
if (WIN32)
  # Use UTF-16 entry points (wmain / wWinMain) and convert argv safely.
  target_link_options(ustudio PRIVATE -municode)

  # In Debug/RelWithDebInfo, build as console so stdout/stderr is visible;
  # In Release/MinSizeRel, build as GUI app.
  if (CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    target_link_options(ustudio PRIVATE -Wl,--subsystem,console)
    if (MINGW)
      # Explicitly choose the Unicode console entry to avoid WinMain mismatch
      target_link_options(ustudio PRIVATE -Wl,-e,wmainCRTStartup)
    endif()
  else()
    target_link_options(ustudio PRIVATE -Wl,--subsystem,windows)
    if (MINGW)
      # Explicitly choose the Unicode GUI entry to avoid WinMain mismatch
      target_link_options(ustudio PRIVATE -Wl,-e,wWinMainCRTStartup)
    endif()
  endif()
endif()

# --- Install (optional) ------------------------------------------------------
# install(TARGETS ustudio RUNTIME DESTINATION bin)
