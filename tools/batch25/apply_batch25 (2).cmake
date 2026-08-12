#-----------------------------------------------------------------------------
# Umicom Studio / Umicom Framework
# File: tools/batch25/apply_batch25.cmake
#
# PURPOSE:
#   Apply the Batch 25 build-integration repair to the REAL Framework and
#   Studio CMakeLists.txt files.  Batch 24 accidentally left its intended
#   CMake changes in *.batch24.merge.txt guide files, so GitHub CI still links
#   the old source lists.  This helper performs exact, guarded replacements.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)

# BEGINNER NOTE:
# Run this file from the Umicom Studio repository root, exactly as shown in
# README_FIRST.txt.  It refuses to continue when an expected anchor is missing,
# instead of guessing where to edit your project.

if(NOT DEFINED STAGE)
    message(FATAL_ERROR
        "STAGE is required. Use -DSTAGE=framework, -DSTAGE=studio, or -DSTAGE=check")
endif()

get_filename_component(BATCH25_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
get_filename_component(TOOLS_DIR "${BATCH25_DIR}/.." ABSOLUTE)
get_filename_component(STUDIO_ROOT "${TOOLS_DIR}/.." ABSOLUTE)
set(FRAMEWORK_ROOT "${STUDIO_ROOT}/framework")
set(FRAMEWORK_CMAKE "${FRAMEWORK_ROOT}/CMakeLists.txt")
set(STUDIO_CMAKE "${STUDIO_ROOT}/applications/studio/CMakeLists.txt")
set(VERSION_LOCK "${STUDIO_ROOT}/VERSION_LOCK.json")
set(FRAMEWORK_OLD_GUIDE "${FRAMEWORK_ROOT}/CMakeLists.txt.batch24.merge.txt")
set(STUDIO_OLD_GUIDE "${STUDIO_ROOT}/applications/studio/CMakeLists.txt.batch24.merge.txt")

macro(umi_require_file path label)
    if(NOT EXISTS "${path}")
        message(FATAL_ERROR "Missing ${label}: ${path}")
    endif()
endmacro()

# Replace an exact text block.  If the old form has already disappeared, the
# macro accepts the file only when the complete new form is already present.
# That makes the operation safe to re-run after an interrupted session.
macro(umi_replace_exact variable old_text new_text label)
    string(FIND "${${variable}}" "${old_text}" _umi_old_position)
    if(NOT _umi_old_position EQUAL -1)
        string(REPLACE "${old_text}" "${new_text}" ${variable} "${${variable}}")
        message(STATUS "Batch 25: updated ${label}")
    else()
        string(FIND "${${variable}}" "${new_text}" _umi_new_position)
        if(_umi_new_position EQUAL -1)
            message(FATAL_ERROR
                "Batch 25 could not locate the expected '${label}' block. "
                "Stop here and do not commit. The repository differs from the verified baseline.")
        endif()
        message(STATUS "Batch 25: ${label} already repaired")
    endif()
endmacro()

macro(umi_require_text variable required_text label)
    string(FIND "${${variable}}" "${required_text}" _umi_required_position)
    if(_umi_required_position EQUAL -1)
        message(FATAL_ERROR "Batch 25 validation failed: ${label}")
    endif()
endmacro()

function(umi_git_head repository out_variable)
    execute_process(
        COMMAND git -C "${repository}" rev-parse HEAD
        RESULT_VARIABLE _umi_git_result
        OUTPUT_VARIABLE _umi_git_output
        ERROR_VARIABLE _umi_git_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT _umi_git_result EQUAL 0)
        message(FATAL_ERROR
            "Could not read Git HEAD for ${repository}: ${_umi_git_error}")
    endif()
    string(LENGTH "${_umi_git_output}" _umi_git_length)
    if(NOT _umi_git_length EQUAL 40 OR
       NOT _umi_git_output MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR "Unexpected Git commit ID: '${_umi_git_output}'")
    endif()
    set(${out_variable} "${_umi_git_output}" PARENT_SCOPE)
endfunction()

function(umi_update_json_string variable key value)
    set(_umi_json "${${variable}}")
    string(REGEX MATCH "\"${key}\"[ \t]*:[ \t]*\"[^\"]*\"" _umi_match "${_umi_json}")
    if(_umi_match STREQUAL "")
        message(FATAL_ERROR "VERSION_LOCK.json is missing string key '${key}'")
    endif()
    string(REGEX REPLACE
        "\"${key}\"[ \t]*:[ \t]*\"[^\"]*\""
        "\"${key}\": \"${value}\""
        _umi_json
        "${_umi_json}"
    )
    set(${variable} "${_umi_json}" PARENT_SCOPE)
endfunction()

function(umi_apply_framework)
    umi_require_file("${FRAMEWORK_CMAKE}" "Framework CMakeLists.txt")
    file(READ "${FRAMEWORK_CMAKE}" FW)

    umi_replace_exact(FW
[=[project(
    UmicomFramework
    VERSION 0.5.0
    LANGUAGES C
)]=]
[=[project(
    UmicomFramework
    VERSION 0.6.0
    LANGUAGES C
)]=]
        "Framework project version 0.6.0")

    umi_replace_exact(FW
[=[    src/ui/view_factory.c
    src/ui/workbench.c
    src/ui/application_shell.c]=]
[=[    src/ui/view_factory.c
    src/ui/activity.c
    src/ui/view_container.c
    src/ui/context.c
    src/ui/keybinding.c
    src/ui/quick_access.c
    src/ui/breadcrumb.c
    src/ui/explorer.c
    src/ui/workbench_state.c
    src/ui/view_presentation.c
    src/ui/workbench.c
    src/ui/application_shell.c]=]
        "Framework unified-workbench UI sources")

    umi_replace_exact(FW
[=[    adapters/headless/workbench_headless.c
    adapters/headless/panes_headless.c
    adapters/headless/documents_headless.c]=]
[=[    adapters/headless/workbench_headless.c
    adapters/headless/panes_headless.c
    adapters/headless/view_model_headless.c
    adapters/headless/documents_headless.c]=]
        "Framework headless view-model renderer")

    umi_replace_exact(FW
[=[        adapters/gtk4/gtk4_adapter.c
        adapters/gtk4/shell_gtk4.c
        adapters/gtk4/workbench_gtk4.c
        adapters/gtk4/pane_gtk4.c]=]
[=[        adapters/gtk4/gtk4_adapter.c
        adapters/gtk4/shell_gtk4.c
        adapters/gtk4/workbench_gtk4.c
        adapters/gtk4/activity_bar_gtk4.c
        adapters/gtk4/view_container_gtk4.c
        adapters/gtk4/breadcrumb_gtk4.c
        adapters/gtk4/quick_access_gtk4.c
        adapters/gtk4/keybinding_gtk4.c
        adapters/gtk4/view_model_gtk4.c
        adapters/gtk4/pane_gtk4.c]=]
        "Framework GTK4 unified-workbench presenters")

    umi_replace_exact(FW
[=[    endfunction()

    umicom_add_framework_test(
        umicom-framework-tests]=]
[=[    endfunction()

    # Batch 23/24 unified workbench foundation and view presentation.
    umicom_add_framework_test(
        umicom-ui-activity-tests
        framework.ui.activity
        tests/test_ui_activity.c)
    umicom_add_framework_test(
        umicom-ui-view-container-tests
        framework.ui.view_container
        tests/test_ui_view_container.c)
    umicom_add_framework_test(
        umicom-ui-context-tests
        framework.ui.context
        tests/test_ui_context.c)
    umicom_add_framework_test(
        umicom-ui-keybinding-tests
        framework.ui.keybinding
        tests/test_ui_keybinding.c)
    umicom_add_framework_test(
        umicom-ui-quick-access-tests
        framework.ui.quick_access
        tests/test_ui_quick_access.c)
    umicom_add_framework_test(
        umicom-ui-breadcrumb-tests
        framework.ui.breadcrumb
        tests/test_ui_breadcrumb.c)
    umicom_add_framework_test(
        umicom-ui-explorer-tests
        framework.ui.explorer
        tests/test_ui_explorer.c)
    umicom_add_framework_test(
        umicom-ui-workbench-state-tests
        framework.ui.workbench_state
        tests/test_ui_workbench_state.c)
    umicom_add_framework_test(
        umicom-ui-view-presentation-tests
        framework.ui.view_presentation
        tests/test_ui_view_presentation.c)
    umicom_add_framework_test(
        umicom-ui-headless-view-presentation-tests
        framework.ui.headless_view_presentation
        tests/test_ui_headless_view_presentation.c)

    umicom_add_framework_test(
        umicom-framework-tests]=]
        "Framework Batch 23/24 UI tests")

    file(WRITE "${FRAMEWORK_CMAKE}" "${FW}")
    if(EXISTS "${FRAMEWORK_OLD_GUIDE}")
        file(REMOVE "${FRAMEWORK_OLD_GUIDE}")
        message(STATUS "Batch 25: removed obsolete Framework Batch 24 merge guide")
    endif()

    message(STATUS "Batch 25 Framework stage complete: ${FRAMEWORK_CMAKE}")
endfunction()

function(umi_apply_studio)
    umi_require_file("${STUDIO_CMAKE}" "Studio application CMakeLists.txt")
    umi_require_file("${VERSION_LOCK}" "Studio VERSION_LOCK.json")
    umi_git_head("${FRAMEWORK_ROOT}" FW_HEAD)
    umi_git_head("${STUDIO_ROOT}" STUDIO_PARENT_HEAD)

    file(READ "${STUDIO_CMAKE}" ST)

    umi_replace_exact(ST
[=[set(UMICOM_STUDIO_VERSION "0.14.0")]=]
[=[set(UMICOM_STUDIO_VERSION "0.15.0")]=]
        "Studio application version 0.15.0")

    umi_replace_exact(ST
[=[    src/app/view_models.c
    src/app/workbench_commands.c
    src/app/ui_catalogue.c]=]
[=[    src/app/view_models.c
    src/app/workbench_commands.c
    src/app/workbench_shell_catalogue.c
    src/app/workbench_views.c
    src/app/ui_catalogue.c]=]
        "Studio unified-workbench composition sources")

    # There are two product compile-definition occurrences in this file. A
    # global exact token replacement is intentional and guarded by the new form.
    string(FIND "${ST}" "UMICOM_VERSION_MINOR=14" _old_minor)
    if(NOT _old_minor EQUAL -1)
        string(REPLACE "UMICOM_VERSION_MINOR=14" "UMICOM_VERSION_MINOR=15" ST "${ST}")
        message(STATUS "Batch 25: updated Studio product compile-definition version")
    else()
        umi_require_text(ST "UMICOM_VERSION_MINOR=15" "Studio minor compile definition is not 15")
    endif()

    umi_replace_exact(ST
[=[    add_executable(umicom-studio-ide
        src/gtk/main.c
    )]=]
[=[    add_executable(umicom-studio-ide
        src/gtk/main.c
        src/gui/workbench/workbench_window.c
    )]=]
        "Studio IDE Framework-backed workbench window source")

    # Keep the CMake ${...} expressions literal. Passing this block through a
    # macro parameter would cause CMake script-mode to expand them too early.
    set(_studio_include_old [=[    target_include_directories(umicom-studio-ide PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"]=])
    set(_studio_include_new [=[    target_include_directories(umicom-studio-ide PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/workbench/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"]=])
    string(FIND "${ST}" "${_studio_include_old}" _studio_include_old_pos)
    if(NOT _studio_include_old_pos EQUAL -1)
        string(REPLACE "${_studio_include_old}" "${_studio_include_new}" ST "${ST}")
        message(STATUS "Batch 25: updated Studio IDE workbench include directory")
    else()
        string(FIND "${ST}" "${_studio_include_new}" _studio_include_new_pos)
        if(_studio_include_new_pos EQUAL -1)
            message(FATAL_ERROR
                "Batch 25 could not locate the Studio IDE include-directory block. "
                "Stop here and do not commit.")
        endif()
        message(STATUS "Batch 25: Studio IDE workbench include directory already repaired")
    endif()

    umi_replace_exact(ST
[=[    target_link_libraries(umicom-studio-ide PRIVATE
        Umicom::StudioCore
        Umicom::StudioProduct
        PkgConfig::GTK4]=]
[=[    target_link_libraries(umicom-studio-ide PRIVATE
        Umicom::StudioCore
        Umicom::StudioProduct
        Umicom::ui_gtk4
        PkgConfig::GTK4]=]
        "Studio IDE Framework GTK4 adapter link")

    umi_replace_exact(ST
[=[    add_test(
        NAME studio.framework_bootstrap
        COMMAND umicom-studio-framework-bootstrap-test
    )

    add_executable(umicom-studio-version-test]=]
[=[    add_test(
        NAME studio.framework_bootstrap
        COMMAND umicom-studio-framework-bootstrap-test
    )

    # Batch 23/24 unified-workbench composition and live-view tests.
    foreach(UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST
            workbench_shell_catalogue
            workbench_activity
            workbench_keybinding
            workbench_session_state
            workbench_views)
        add_executable(umicom-studio-${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}-test
            tests/test_${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}.c)
        target_link_libraries(
            umicom-studio-${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(
            umicom-studio-${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}-test)
        umicom_apply_sanitizers(
            umicom-studio-${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}-test)
        add_test(
            NAME studio.unified_workbench.${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_UNIFIED_WORKBENCH_TEST}-test)
    endforeach()

    add_executable(umicom-studio-version-test]=]
        "Studio Batch 23/24 unified-workbench tests")

    file(WRITE "${STUDIO_CMAKE}" "${ST}")

    file(READ "${VERSION_LOCK}" LOCK_JSON)
    umi_update_json_string(LOCK_JSON "studio_version" "0.15.0")
    umi_update_json_string(LOCK_JSON "studio_parent_base_commit" "${STUDIO_PARENT_HEAD}")
    umi_update_json_string(LOCK_JSON "framework_version" "0.6.0")
    umi_update_json_string(LOCK_JSON "framework_commit" "${FW_HEAD}")
    file(WRITE "${VERSION_LOCK}" "${LOCK_JSON}")

    if(EXISTS "${STUDIO_OLD_GUIDE}")
        file(REMOVE "${STUDIO_OLD_GUIDE}")
        message(STATUS "Batch 25: removed obsolete Studio Batch 24 merge guide")
    endif()

    message(STATUS "Batch 25 Studio stage complete: ${STUDIO_CMAKE}")
    message(STATUS "Batch 25 VERSION_LOCK Framework commit: ${FW_HEAD}")
    message(STATUS "Batch 25 Studio parent baseline: ${STUDIO_PARENT_HEAD}")
endfunction()

function(umi_check)
    umi_require_file("${FRAMEWORK_CMAKE}" "Framework CMakeLists.txt")
    umi_require_file("${STUDIO_CMAKE}" "Studio application CMakeLists.txt")
    umi_require_file("${VERSION_LOCK}" "Studio VERSION_LOCK.json")

    file(READ "${FRAMEWORK_CMAKE}" FW)
    file(READ "${STUDIO_CMAKE}" ST)
    file(READ "${VERSION_LOCK}" LOCK_JSON)

    foreach(_fw_required
        "VERSION 0.6.0"
        "src/ui/activity.c"
        "src/ui/view_container.c"
        "src/ui/context.c"
        "src/ui/keybinding.c"
        "src/ui/quick_access.c"
        "src/ui/breadcrumb.c"
        "src/ui/explorer.c"
        "src/ui/workbench_state.c"
        "src/ui/view_presentation.c"
        "adapters/headless/view_model_headless.c"
        "adapters/gtk4/activity_bar_gtk4.c"
        "adapters/gtk4/view_container_gtk4.c"
        "adapters/gtk4/breadcrumb_gtk4.c"
        "adapters/gtk4/quick_access_gtk4.c"
        "adapters/gtk4/keybinding_gtk4.c"
        "adapters/gtk4/view_model_gtk4.c"
        "framework.ui.headless_view_presentation")
        umi_require_text(FW "${_fw_required}" "Framework is missing ${_fw_required}")
    endforeach()

    foreach(_studio_required
        "set(UMICOM_STUDIO_VERSION \"0.15.0\")"
        "src/app/workbench_shell_catalogue.c"
        "src/app/workbench_views.c"
        "src/gui/workbench/workbench_window.c"
        "src/gui/workbench/include"
        "Umicom::ui_gtk4"
        "UMICOM_VERSION_MINOR=15"
        "studio.unified_workbench")
        umi_require_text(ST "${_studio_required}" "Studio is missing ${_studio_required}")
    endforeach()

    if(EXISTS "${FRAMEWORK_OLD_GUIDE}")
        message(FATAL_ERROR "Obsolete Framework CMakeLists.txt.batch24.merge.txt still exists")
    endif()
    if(EXISTS "${STUDIO_OLD_GUIDE}")
        message(FATAL_ERROR "Obsolete Studio CMakeLists.txt.batch24.merge.txt still exists")
    endif()

    umi_git_head("${FRAMEWORK_ROOT}" FW_HEAD)
    string(FIND "${LOCK_JSON}" "\"studio_version\": \"0.15.0\"" _studio_version_ok)
    string(FIND "${LOCK_JSON}" "\"framework_version\": \"0.6.0\"" _framework_version_ok)
    string(FIND "${LOCK_JSON}" "\"framework_commit\": \"${FW_HEAD}\"" _framework_head_ok)
    if(_studio_version_ok EQUAL -1 OR
       _framework_version_ok EQUAL -1 OR
       _framework_head_ok EQUAL -1)
        message(FATAL_ERROR
            "VERSION_LOCK.json does not match Studio 0.15.0 / Framework 0.6.0 / current Framework HEAD")
    endif()

    message(STATUS "Batch 25 structural validation: PASS")
    message(STATUS "Framework HEAD recorded by Studio: ${FW_HEAD}")
endfunction()

if(STAGE STREQUAL "framework")
    umi_apply_framework()
elseif(STAGE STREQUAL "studio")
    umi_apply_studio()
elseif(STAGE STREQUAL "check")
    umi_check()
else()
    message(FATAL_ERROR "Unknown STAGE='${STAGE}'. Use framework, studio, or check.")
endif()
