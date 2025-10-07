cmake_minimum_required(VERSION 3.22)
project(umicom-studio LANGUAGES C VERSION 0.1.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(JSONGLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)

file(GLOB_RECURSE SRC_CORE   CONFIGURE_DEPENDS src/core/*.c)
file(GLOB_RECURSE SRC_GUI    CONFIGURE_DEPENDS src/gui/*.c)
file(GLOB_RECURSE SRC_EDITOR CONFIGURE_DEPENDS src/editor/*.c)
file(GLOB_RECURSE SRC_BUILD  CONFIGURE_DEPENDS src/build/*.c)
file(GLOB_RECURSE SRC_SEARCH CONFIGURE_DEPENDS src/search/*.c)
file(GLOB_RECURSE SRC_LLM    CONFIGURE_DEPENDS src/llm/*.c)

add_executable(ustudio
  ${SRC_CORE} ${SRC_GUI} ${SRC_EDITOR} ${SRC_BUILD} ${SRC_SEARCH} ${SRC_LLM}
  src/main.c
)
target_include_directories(ustudio PRIVATE include src)
target_link_libraries(ustudio
  PkgConfig::GTK4 PkgConfig::GLIB PkgConfig::JSONGLIB PkgConfig::CURL
)

enable_testing()
add_executable(test_llm_body tests/test_llm_body.c)
target_include_directories(test_llm_body PRIVATE include)
add_test(NAME test_llm_body COMMAND test_llm_body)

add_executable(test_llm_json tests/test_llm_json.c)
target_link_libraries(test_llm_json PkgConfig::JSONGLIB)
add_test(NAME test_llm_json COMMAND test_llm_json)

install(TARGETS ustudio RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

# ---- Begin auto-added by scripts/apply-umicom-fixes.ps1 ----
include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GLIB REQUIRED glib-2.0)
  if (TARGET test_llm_body)
    target_include_directories(test_llm_body PRIVATE ${GLIB_INCLUDE_DIRS})
    target_link_libraries(test_llm_body PRIVATE ${GLIB_LIBRARIES})
  endif()
endif()

if (TARGET ustudio)
  target_include_directories(ustudio PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
endif()
# ---- End auto-added by scripts/apply-umicom-fixes.ps1 ----


