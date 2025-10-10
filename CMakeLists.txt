/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: CMakeLists.txt
 * PURPOSE: Top-level build script (clean & warnings tuned for MinGW/GTK4).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

cmake_minimum_required(VERSION 3.20)
project(umicom-studio-ide C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(USIDE_DEV_CONSOLE "Build with Windows console subsystem so stdout/stderr appear" ON)

file(GLOB_RECURSE USIDE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

message(STATUS "umicom-studio sources: ${CMAKE_SOURCE_DIR}")
list(LENGTH USIDE_SOURCES NUM_SOURCES)
message(STATUS "Number of C sources: ${NUM_SOURCES}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(GDKPIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(HARFBUZZ REQUIRED IMPORTED_TARGET harfbuzz)
pkg_check_modules(GRAPHENE REQUIRED IMPORTED_TARGET graphene-1.0)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSONGLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(LIBCURL REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(SOUP3 REQUIRED IMPORTED_TARGET libsoup-3.0)

add_executable(ustudio ${USIDE_SOURCES})

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

target_include_directories(ustudio SYSTEM PRIVATE
    ${GTK4_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${GDKPIXBUF_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${GRAPHENE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${JSONGLIB_INCLUDE_DIRS}
    ${LIBCURL_INCLUDE_DIRS}
    ${SOUP3_INCLUDE_DIRS}
)

target_link_libraries(ustudio PRIVATE
    PkgConfig::GTK4
    PkgConfig::PANGO
    PkgConfig::GDKPIXBUF
    PkgConfig::CAIRO
    PkgConfig::HARFBUZZ
    PkgConfig::GRAPHENE
    PkgConfig::GLIB
    PkgConfig::JSONGLIB
    PkgConfig::LIBCURL
    PkgConfig::SOUP3
    kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32
)

target_compile_definitions(ustudio PRIVATE
    _UNICODE UNICODE
    __USE_MINGW_ANSI_STDIO=1
)

target_compile_options(ustudio PRIVATE
    -Wall -Wextra -Wno-missing-field-initializers
    -Wno-deprecated-declarations
)

if(MINGW)
  target_link_options(ustudio PRIVATE -municode)
  if(USIDE_DEV_CONSOLE)
    target_link_options(ustudio PRIVATE -Wl,--subsystem,console)
    target_compile_definitions(ustudio PRIVATE USIDE_DEV_CONSOLE=1)
  else()
    target_link_options(ustudio PRIVATE -Wl,--subsystem,windows)
  endif()
endif()

install(TARGETS ustudio RUNTIME DESTINATION ".")
