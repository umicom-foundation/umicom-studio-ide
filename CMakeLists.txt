# ---------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
# PURPOSE: Top-level CMake configuration
# Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.22)
project(umicom-studio LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings common across platforms
add_compile_options(-Wall -Wextra -Wno-deprecated-declarations)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(JSONGLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)

# Collect sources
file(GLOB_RECURSE SRC_CORE   CONFIGURE_DEPENDS src/core/*.c)
file(GLOB_RECURSE SRC_GUI    CONFIGURE_DEPENDS src/gui/*.c)
file(GLOB_RECURSE SRC_EDITOR CONFIGURE_DEPENDS src/editor/*.c)
file(GLOB_RECURSE SRC_BUILD  CONFIGURE_DEPENDS src/build/*.c)
file(GLOB_RECURSE SRC_SEARCH CONFIGURE_DEPENDS src/search/*.c)
file(GLOB_RECURSE SRC_LLM    CONFIGURE_DEPENDS src/llm/*.c)

# Top-level C files (but don't accidentally double-compile files whose canonical
# versions live under src/core, and don't include main.c here because we add it explicitly).
file(GLOB SRC_TOP CONFIGURE_DEPENDS src/*.c)
# Remove duplicates that cause multiply-defined symbols if a file also exists in src/core
list(FILTER SRC_TOP EXCLUDE REGEX ".*/umi_log\\.c$")
# Ensure we don't add main.c twice
list(FILTER SRC_TOP EXCLUDE REGEX ".*/main\\.c$")

add_executable(ustudio
  ${SRC_CORE} ${SRC_GUI} ${SRC_EDITOR} ${SRC_BUILD} ${SRC_SEARCH} ${SRC_LLM}
  ${SRC_TOP}
  src/main.c
)

target_include_directories(ustudio PRIVATE include src)
target_link_libraries(ustudio
  PkgConfig::GTK4 PkgConfig::GLIB PkgConfig::JSONGLIB PkgConfig::CURL
)

enable_testing()

# Optional tests (existence-guarded so the project builds even without them)
if (EXISTS "${CMAKE_SOURCE_DIR}/tests/test_llm_body.c")
  add_executable(test_llm_body tests/test_llm_body.c)
  target_include_directories(test_llm_body PRIVATE include)
  if (TARGET PkgConfig::GLIB)
    target_link_libraries(test_llm_body PRIVATE PkgConfig::GLIB)
  endif()
  add_test(NAME test_llm_body COMMAND test_llm_body)
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/tests/test_llm_json.c")
  add_executable(test_llm_json tests/test_llm_json.c)
  target_link_libraries(test_llm_json PkgConfig::JSONGLIB)
  add_test(NAME test_llm_json COMMAND test_llm_json)
endif()

install(TARGETS ustudio RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
