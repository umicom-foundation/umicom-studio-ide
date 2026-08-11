#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: applications/studio/workspace-productivity/CMakeLists.txt
#
# PURPOSE:
#   Build the bounded workspace/editor productivity library, headless inspector
#   and GTK4/GtkSourceView workbench.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
set(UMICOM_STUDIO_WORKSPACE_PRODUCTIVITY_SOURCES
    src/workspace_document.c
    src/document_set.c
    src/recent_workspaces.c
    src/quick_open.c
    src/navigation_history.c
    src/find_replace.c
    src/session_state.c
    src/editor_status.c
    src/workspace_summary.c
    src/tab_policy.c
)

add_library(umicom_studio_workspace_productivity STATIC
    ${UMICOM_STUDIO_WORKSPACE_PRODUCTIVITY_SOURCES}
)
add_library(Umicom::StudioWorkspaceProductivity ALIAS
    umicom_studio_workspace_productivity)

target_include_directories(umicom_studio_workspace_productivity PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(umicom_studio_workspace_productivity PUBLIC
    Umicom::base
)

umicom_apply_warnings(umicom_studio_workspace_productivity)
umicom_apply_sanitizers(umicom_studio_workspace_productivity)

add_executable(umicom-studio-workspace-tools
    cli/main.c
)
target_link_libraries(umicom-studio-workspace-tools PRIVATE
    Umicom::StudioWorkspaceProductivity
)
umicom_apply_warnings(umicom-studio-workspace-tools)
umicom_apply_sanitizers(umicom-studio-workspace-tools)

if(UMICOM_STUDIO_BUILD_GTK)
    find_package(PkgConfig REQUIRED)

    if(NOT TARGET PkgConfig::WORKSPACE_GTK4)
        pkg_check_modules(WORKSPACE_GTK4 REQUIRED IMPORTED_TARGET gtk4)
    endif()

    if(NOT TARGET PkgConfig::WORKSPACE_SOURCEVIEW5)
        pkg_check_modules(WORKSPACE_SOURCEVIEW5 QUIET IMPORTED_TARGET
            gtksourceview-5)
    endif()

    add_executable(umicom-studio-editor-workbench
        gtk/main.c
        gtk/editor_workbench_window.c
    )

    target_include_directories(umicom-studio-editor-workbench PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/gtk/include"
    )

    target_link_libraries(umicom-studio-editor-workbench PRIVATE
        Umicom::StudioWorkspaceProductivity
        PkgConfig::WORKSPACE_GTK4
    )

    if(TARGET PkgConfig::WORKSPACE_SOURCEVIEW5)
        target_link_libraries(umicom-studio-editor-workbench PRIVATE
            PkgConfig::WORKSPACE_SOURCEVIEW5
        )
        target_compile_definitions(umicom-studio-editor-workbench PRIVATE
            UMICOM_STUDIO_HAS_SOURCEVIEW=1
        )
    endif()

    umicom_apply_warnings(umicom-studio-editor-workbench)
    umicom_apply_sanitizers(umicom-studio-editor-workbench)

    install(TARGETS umicom-studio-editor-workbench
        RUNTIME DESTINATION bin
    )
endif()

install(TARGETS umicom-studio-workspace-tools
    RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
    foreach(UMICOM_STUDIO_WORKSPACE_TEST
            document_set
            preview_replacement
            dirty_close
            recent_workspaces
            quick_open
            navigation_history
            find_replace
            find_case
            session_state
            editor_status
            workspace_summary
            tab_policy)
        add_executable(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST}-test
            tests/test_${UMICOM_STUDIO_WORKSPACE_TEST}.c
        )
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST}-test
            PRIVATE Umicom::StudioWorkspaceProductivity
        )
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST}-test)
        add_test(
            NAME studio.workspace_productivity.${UMICOM_STUDIO_WORKSPACE_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST}-test
        )
    endforeach()
endif()
