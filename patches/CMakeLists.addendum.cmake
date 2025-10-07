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
