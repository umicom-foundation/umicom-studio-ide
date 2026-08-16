#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: applications/studio/cmake/UmicomWorkspaceSearchContribution.cmake
#
# PURPOSE:
#   Add the thin Studio placement contribution for Framework-owned workspace
#   search and replace capabilities, plus its architecture-boundary test.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

if(NOT TARGET umicom_studio_core)
    message(FATAL_ERROR
        "UmicomWorkspaceSearchContribution.cmake must be included after umicom_studio_core is created")
endif()

target_sources(umicom_studio_core PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/app/workspace_search_contribution.c")

if(BUILD_TESTING)
    add_executable(umicom_test_studio_workspace_search_contribution
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_workspace_search_contribution.c")
    target_link_libraries(umicom_test_studio_workspace_search_contribution
        PRIVATE Umicom::StudioCore Umicom::editor)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(
            umicom_test_studio_workspace_search_contribution)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(
            umicom_test_studio_workspace_search_contribution)
    endif()
    add_test(
        NAME studio.workspace_search_contribution
        COMMAND umicom_test_studio_workspace_search_contribution)
    set_tests_properties(studio.workspace_search_contribution PROPERTIES
        LABELS "studio;editor;workspace-search;batch-93")
endif()
