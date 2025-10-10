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
project(umicom_studio_ide C)

# --- Options -----------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(USIDE_DEV_CONSOLE "Build with console subsystem (stdout/stderr visible)" ON)

# --- Dependencies -------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4     REQUIRED gtk4)
pkg_check_modules(PANGO    REQUIRED pango)
pkg_check_modules(PIXBUF   REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(CAIRO    REQUIRED cairo)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(GRAPHENE REQUIRED graphene-1.0)
pkg_check_modules(GLIB     REQUIRED glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(JSONGLIB REQUIRED json-glib-1.0)
pkg_check_modules(LIBCURL  REQUIRED libcurl)
pkg_check_modules(SOUP3    REQUIRED libsoup-3.0)

# --- Sources -----------------------------------------------------------------
# Keep this glob; it mirrors the repo structure and is easy to maintain.
file(GLOB_RECURSE USIDE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# If you want a *cleaner* build, you can exclude known throwaway/demo files:
# list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/gtk_smoke\.c$")
# list(FILTER USIDE_SOURCES EXCLUDE REGEX ".*/test_scaffold\.c$")

message(STATUS "Config type: ${CMAKE_BUILD_TYPE}")
message(STATUS "USIDE_DEV_CONSOLE: ${USIDE_DEV_CONSOLE}")
message(STATUS "Number of C sources: ${USIDE_SOURCES}")

add_executable(ustudio ${USIDE_SOURCES})

# --- Include directories (do NOT treat these as -D macros!) -------------------
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
    "${CMAKE_SOURCE_DIR}/SYSTEM"             # for local shim headers
    ${GTK4_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${PIXBUF_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    ${GRAPHENE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${JSONGLIB_INCLUDE_DIRS}
    ${LIBCURL_INCLUDE_DIRS}
    ${SOUP3_INCLUDE_DIRS}
)

# --- Link libraries -----------------------------------------------------------
target_link_libraries(ustudio PRIVATE
    ${GTK4_LIBRARIES}
    ${PANGO_LIBRARIES}
    ${PIXBUF_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${HARFBUZZ_LIBRARIES}
    ${GRAPHENE_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${JSONGLIB_LIBRARIES}
    ${LIBCURL_LIBRARIES}
    ${SOUP3_LIBRARIES}
)

# --- Compiler & linker flags --------------------------------------------------
target_compile_options(ustudio PRIVATE
    ${GTK4_CFLAGS_OTHER}
    ${PANGO_CFLAGS_OTHER}
    ${PIXBUF_CFLAGS_OTHER}
    ${CAIRO_CFLAGS_OTHER}
    ${HARFBUZZ_CFLAGS_OTHER}
    ${GRAPHENE_CFLAGS_OTHER}
    ${GLIB_CFLAGS_OTHER}
    ${JSONGLIB_CFLAGS_OTHER}
    ${LIBCURL_CFLAGS_OTHER}
    ${SOUP3_CFLAGS_OTHER}
    -Wall -Wextra -Wno-missing-field-initializers
    -Wno-deprecated-declarations           # silence known GTK deprecations we still use
)

if (MSYS OR MINGW)
    # Use wide-char entry points; SSE flags match your existing toolchain.
    target_compile_options(ustudio PRIVATE -municode -mfpmath=sse -msse -msse2)

    if (USIDE_DEV_CONSOLE)
        # Console subsystem -> use main() and see logs in a console window.
        target_link_options(ustudio PRIVATE -Wl,--subsystem,console)
    else()
        # GUI subsystem -> requires WinMain/wWinMain (provided by src/win_launcher.c).
        target_link_options(ustudio PRIVATE -Wl,--subsystem,windows)
    endif()
endif()

# --- Public feature defines ---------------------------------------------------
if (USIDE_DEV_CONSOLE)
    target_compile_definitions(ustudio PRIVATE USIDE_DEV_CONSOLE=1)
else()
    target_compile_definitions(ustudio PRIVATE USIDE_GUI_BUILD=1)
endif()
