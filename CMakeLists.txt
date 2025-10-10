# -----------------------------------------------------------------------------
# Umicom Studio IDE (USIDE)
# File: CMakeLists.txt
# PURPOSE: Project build system (MinGW/Windows + GTK4). Provides a debug
#          console mode for development and a GUI subsystem for releases.
#
# Created by: Umicom Foundation (https://umicom.foundation/)
# Author: Sammy Hegab (with build fixes)
# Date: 15-09-2025
# License: MIT
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)
project(umicom-studio-ide C)

# C standard and warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(USIDE_DEV_CONSOLE "Build as console app on Windows (helpful for logs)" ON)

# Dependencies (GTK4 stack)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(GRAPHENE REQUIRED graphene-1.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSONGLIB REQUIRED json-glib-1.0)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(SOUP REQUIRED libsoup-3.0)

# Sources
file(GLOB_RECURSE USIDE_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Avoid accidentally picking a duplicated Windows launcher under src/src/
# (just in case an old copy exists).
list(FILTER USIDE_SRC EXCLUDE REGEX ".*/src/src/.*")

# Choose subsystem on Windows
if(WIN32 AND NOT USIDE_DEV_CONSOLE)
    add_definitions(-DUSIDE_GUI_BUILD=1)
    add_executable(ustudio WIN32 ${USIDE_SRC})
else()
    add_executable(ustudio ${USIDE_SRC})
endif()

target_include_directories(ustudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/panes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/transpile
    ${CMAKE_CURRENT_SOURCE_DIR}/src/build
    ${CMAKE_CURRENT_SOURCE_DIR}/src/llm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/llm/providers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/search
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    SYSTEM ${GTK4_INCLUDE_DIRS} ${PANGO_INCLUDE_DIRS} ${GDKPIXBUF_INCLUDE_DIRS}
           ${CAIRO_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${GRAPHENE_INCLUDE_DIRS}
           ${GLIB2_INCLUDE_DIRS} ${JSONGLIB_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
           ${SOUP_INCLUDE_DIRS}
)

# Compile options
target_compile_options(ustudio PRIVATE
    -Wall -Wextra -Wno-missing-field-initializers
)

# Link
target_link_directories(ustudio PRIVATE
    ${GTK4_LIBRARY_DIRS} ${PANGO_LIBRARY_DIRS} ${GDKPIXBUF_LIBRARY_DIRS}
    ${CAIRO_LIBRARY_DIRS} ${HARFBUZZ_LIBRARY_DIRS} ${GRAPHENE_LIBRARY_DIRS}
    ${GLIB2_LIBRARY_DIRS} ${JSONGLIB_LIBRARY_DIRS} ${CURL_LIBRARY_DIRS}
    ${SOUP_LIBRARY_DIRS}
)

target_link_libraries(ustudio PRIVATE
    ${GTK4_LIBRARIES} ${PANGO_LIBRARIES} ${GDKPIXBUF_LIBRARIES}
    ${CAIRO_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${GRAPHENE_LIBRARIES}
    ${GLIB2_LIBRARIES} ${JSONGLIB_LIBRARIES} ${CURL_LIBRARIES}
    ${SOUP_LIBRARIES}
)

# Windows specifics
if(WIN32)
    # Only use -municode for GUI builds; console builds rely on main()
    if(NOT USIDE_DEV_CONSOLE)
        target_link_options(ustudio PRIVATE -municode -Wl,--subsystem,windows)
    endif()
    # Base Win32 libs
    target_link_libraries(ustudio PRIVATE
        kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32
    )
endif()

# Nice compile_commands.json for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
