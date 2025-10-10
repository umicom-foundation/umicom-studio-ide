# ---------------------------------------------------------------------------
# Umicom Studio IDE (USIDE)
# File: CMakeLists.txt
# PURPOSE: Build system for MSYS2/MinGW (Windows) and other platforms.
#
# Created by: Umicom Foundation (https://umicom.foundation/)
# Author: Sammy Hegab
# Date: 15-09-2025
# License: MIT
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
project(umicom-studio-ide C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option: build without a console (Windows GUI subsystem)
option(USTUDIO_GUI "Build as a GUI subsystem app (no console window)" OFF)

# Dependencies (via pkg-config on MSYS2)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(GRAPHENE REQUIRED graphene-1.0)
pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSONGLIB REQUIRED json-glib-1.0)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(SOUP3 REQUIRED libsoup-3.0)

# Source files
file(GLOB_RECURSE UMI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# Guard against accidentally added duplicate/backup dirs
list(FILTER UMI_SOURCES EXCLUDE REGEX ".*/src/src/.*")
list(FILTER UMI_SOURCES EXCLUDE REGEX ".*/(backup|old|deprecated|experimental)/.*")

# Always compile our known-safe Windows launcher (prevents recursion/stack overflow)
# Ensure we keep the copy under src/win_launcher.c and ignore any duplicates elsewhere.
set(LAUNCHER_FILE "${CMAKE_SOURCE_DIR}/src/win_launcher.c")
list(REMOVE_ITEM UMI_SOURCES "${CMAKE_SOURCE_DIR}/src/src/win_launcher.c")
if(NOT UMI_SOURCES MATCHES "${LAUNCHER_FILE}")
  list(APPEND UMI_SOURCES "${LAUNCHER_FILE}")
endif()

# Executable
add_executable(ustudio ${UMI_SOURCES})

# Include paths
target_include_directories(ustudio PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    ${GTK4_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${GDKPIXBUF_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${GRAPHENE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${JSONGLIB_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${SOUP3_INCLUDE_DIRS}
)

# Libraries
target_link_libraries(ustudio PRIVATE
    ${GTK4_LIBRARIES}
    ${PANGO_LIBRARIES}
    ${GDKPIXBUF_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${HARFBUZZ_LIBRARIES}
    ${GRAPHENE_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${JSONGLIB_LIBRARIES}
    ${CURL_LIBRARIES}
    ${SOUP3_LIBRARIES}
    kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32
)

# Compiler warnings (keep callbacks clean but don't block on benign warnings)
if(MSVC)
  target_compile_options(ustudio PRIVATE /W4)
else()
  target_compile_options(ustudio PRIVATE -Wall -Wextra -Wno-missing-field-initializers)
endif()

# Windows entry configuration
if(MINGW OR WIN32)
  # Accept UTF-16 argv and forward to UTF-8 main()
  target_link_options(ustudio PRIVATE -municode)

  if(USTUDIO_GUI)
    target_link_options(ustudio PRIVATE -Wl,--subsystem,windows)
  else()
    # Console by default during development so stdout/stderr are visible
    target_link_options(ustudio PRIVATE -Wl,--subsystem,console)
  endif()
endif()

message(STATUS "umicom-studio sources: ${CMAKE_SOURCE_DIR}")
message(STATUS "Number of C sources: ${UMI_SOURCES_LENGTH}")
