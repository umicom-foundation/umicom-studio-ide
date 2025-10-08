# ---------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Top-level CMake configuration
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.22)

project(umicom-studio
  LANGUAGES C
  VERSION 0.1.0
)

# Language / tooling
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4     REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GLIB     REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(JSONGLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(CURL     REQUIRED IMPORTED_TARGET libcurl)

# ------------------------------------------------------------------------------
# Sources
# We glob everything under src/, then explicitly exclude the temporary shim
# src/umi_log.c so we only build the real implementation in src/core/umi_log.c.
# ------------------------------------------------------------------------------

file(GLOB_RECURSE SRC_ALL CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# If the accidental shim exists, remove it from the list to avoid duplicate symbols.
list(FILTER SRC_ALL EXCLUDE REGEX ".*/src/umi_log\\.c$")

# ------------------------------------------------------------------------------
# Target
# ------------------------------------------------------------------------------

add_executable(ustudio
  ${SRC_ALL}
)

# Include paths so headers like "core/umi_log.h" and your public headers are found.
target_include_directories(ustudio PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Common compile definitions seen in your build logs
target_compile_definitions(ustudio PRIVATE
  __USE_MINGW_ANSI_STDIO=1
  LIBDEFLATE_DLL
)

# Warnings are helpful; feel free to tweak
if (MSVC)
  target_compile_options(ustudio PRIVATE /W3)
else()
  target_compile_options(ustudio PRIVATE -Wall -Wextra -Wno-deprecated-declarations)
endif()

# Link dependencies discovered via pkg-config
target_link_libraries(ustudio PRIVATE
  PkgConfig::GTK4
  PkgConfig::GLIB
  PkgConfig::JSONGLIB
  PkgConfig::CURL
)

# On MinGW/Windows, lld and system libs come from the toolchain; nothing special here.

# Nice-to-haves for IDEs
set_property(TARGET ustudio PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "umicom-studio sources: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Number of C sources: ${SRC_ALL_LENGTH}")
