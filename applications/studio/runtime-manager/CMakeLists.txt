#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: applications/studio/runtime-manager/CMakeLists.txt
#
# PURPOSE:
#   Build the Studio Application Hub / Runtime Manager as a bounded Studio
#   feature over the public Umicom Framework integration runtime.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
set(UMICOM_STUDIO_RUNTIME_MANAGER_SOURCES
    src/runtime_manager.c
    src/runtime_catalogue.c
    src/runtime_filter.c
    src/runtime_snapshot.c
    src/runtime_launch_plan.c
)

add_library(umicom_studio_runtime_manager STATIC
    ${UMICOM_STUDIO_RUNTIME_MANAGER_SOURCES}
)
add_library(Umicom::StudioRuntimeManager ALIAS umicom_studio_runtime_manager)

target_include_directories(umicom_studio_runtime_manager PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(umicom_studio_runtime_manager PUBLIC
    Umicom::integration
)

umicom_apply_warnings(umicom_studio_runtime_manager)
umicom_apply_sanitizers(umicom_studio_runtime_manager)

add_executable(umicom-studio-apps
    cli/main.c
)
target_link_libraries(umicom-studio-apps PRIVATE
    Umicom::StudioRuntimeManager
)
umicom_apply_warnings(umicom-studio-apps)
umicom_apply_sanitizers(umicom-studio-apps)

if(UMICOM_STUDIO_BUILD_GTK)
    find_package(PkgConfig REQUIRED)

    if(NOT TARGET PkgConfig::GTK4)
        pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
    endif()

    add_executable(umicom-studio-app-hub
        gtk/app_hub_main.c
        gtk/app_hub_window.c
    )

    target_include_directories(umicom-studio-app-hub PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/gtk/include"
    )

    target_link_libraries(umicom-studio-app-hub PRIVATE
        Umicom::StudioRuntimeManager
        PkgConfig::GTK4
    )

    umicom_apply_warnings(umicom-studio-app-hub)
    umicom_apply_sanitizers(umicom-studio-app-hub)

    install(TARGETS umicom-studio-app-hub
        RUNTIME DESTINATION bin
    )
endif()

install(TARGETS umicom-studio-apps
    RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
    foreach(UMICOM_STUDIO_RUNTIME_TEST
            runtime_manager
            runtime_find
            runtime_state
            runtime_filter
            runtime_snapshot
            runtime_launch_plan
            runtime_favourite
            runtime_presence
            runtime_executable)
        add_executable(umicom-studio-${UMICOM_STUDIO_RUNTIME_TEST}-test
            tests/test_${UMICOM_STUDIO_RUNTIME_TEST}.c
        )
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_RUNTIME_TEST}-test
            PRIVATE Umicom::StudioRuntimeManager
        )
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_RUNTIME_TEST}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_RUNTIME_TEST}-test)
        add_test(
            NAME studio.${UMICOM_STUDIO_RUNTIME_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_RUNTIME_TEST}-test
        )
    endforeach()
endif()
