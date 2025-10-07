# ---------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Top-level CMake configuration
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
project(ustudio C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Prefer Ninja if the generator wasn't chosen
if(NOT CMAKE_GENERATOR)
  set(CMAKE_GENERATOR "Ninja" CACHE STRING "" FORCE)
endif()

# Dependencies via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(JSON REQUIRED json-glib-1.0)
pkg_check_modules(CURL REQUIRED libcurl)

# Include dirs
include_directories(
  ${GTK_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
  ${JSON_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

add_definitions(${GTK_CFLAGS_OTHER} ${GLIB_CFLAGS_OTHER} ${JSON_CFLAGS_OTHER})

# Sources: grab everything in src/ (subdirs) and the llm providers
file(GLOB_RECURSE SRC_ALL CONFIGURE_DEPENDS
  ${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/src/*/*.c
  ${CMAKE_SOURCE_DIR}/src/llm/providers/*.c
)

add_executable(ustudio ${SRC_ALL})

# Compile flags
if(MSVC)
  # (none)
else()
  target_compile_definitions(ustudio PRIVATE __USE_MINGW_ANSI_STDIO=1)
  if(MINGW)
    # Ensure console subsystem so main() links, and LLD works well
    target_link_options(ustudio PRIVATE -Wl,--subsystem,console)
  endif()
endif()

# Link libraries
target_link_libraries(ustudio PRIVATE
  ${GTK_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${JSON_LIBRARIES}
  ${CURL_LIBRARIES}
)

if(MINGW)
  target_link_libraries(ustudio PRIVATE
    mingw32
    user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32
  )
endif()

# Group sources for IDEs
source_group(TREE ${CMAKE_SOURCE_DIR}/src FILES ${SRC_ALL})

# Optional tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
  add_subdirectory(tests)
endif()
# ---------------------------------------------------------------------------
