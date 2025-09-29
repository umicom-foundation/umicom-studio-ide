
cmake_minimum_required(VERSION 3.20)
project(umicom-studio-ide C)

set(CMAKE_C_STANDARD 17)

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# GResource (fallback: use glib-compile-resources at build time)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ustudio-resources.c
  COMMAND glib-compile-resources --generate-source --target=${CMAKE_CURRENT_BINARY_DIR}/ustudio-resources.c ${CMAKE_CURRENT_SOURCE_DIR}/data/ustudio.gresource.xml --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/data
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/ustudio.gresource.xml
)

add_executable(ustudio
  src/main.c
  src/app.c
  src/window.c
  src/studio_codestral_fim.c
  ${CMAKE_CURRENT_BINARY_DIR}/ustudio-resources.c
)

target_include_directories(ustudio PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(ustudio PRIVATE PkgConfig::GTK4)
target_link_libraries(studio_fim_demo PRIVATE CURL::libcurl)