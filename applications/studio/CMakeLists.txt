#-----------------------------------------------------------------------------
# Umicom Studio IDE application targets
# File: applications/studio/CMakeLists.txt
#
# PURPOSE:
#   Create explicit Framework-based Studio targets.  The current Studio source
#   is treated as the active product implementation, not as a disposable legacy
#   library.  Reusable capabilities will continue to migrate into Framework in
#   small, independently tested vertical slices.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
set(UMICOM_STUDIO_VERSION "0.13.0")

# -----------------------------------------------------------------------------
# Studio composition root
# -----------------------------------------------------------------------------
add_library(umicom_studio_core STATIC
    src/app/bootstrap.c
    src/app/services.c
    src/app/version.c
    src/app/doctor.c
    src/app/diagnostics.c
    src/app/settings.c
)
add_library(Umicom::StudioCore ALIAS umicom_studio_core)

target_include_directories(umicom_studio_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(umicom_studio_core PUBLIC
    Umicom::Framework
)

umicom_apply_warnings(umicom_studio_core)
umicom_apply_sanitizers(umicom_studio_core)

# A small console frontend verifies Framework lifecycle without GTK4.
add_executable(umicom-studio-console
    src/console/main.c
)
target_link_libraries(umicom-studio-console PRIVATE
    Umicom::StudioCore
)
umicom_apply_warnings(umicom-studio-console)
umicom_apply_sanitizers(umicom-studio-console)

# Native project-health command.  This is intentionally implemented in C23 so
# normal repository validation does not depend on Python.
add_executable(umicom-studio-doctor
    src/tools/doctor_main.c
)
target_link_libraries(umicom-studio-doctor PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-doctor)
umicom_apply_sanitizers(umicom-studio-doctor)

# Native retained-diagnostics inspection command.
add_executable(umicom-studio-diagnostics
    src/tools/diagnostics_main.c
)
target_link_libraries(umicom-studio-diagnostics PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-diagnostics)
umicom_apply_sanitizers(umicom-studio-diagnostics)


# Native typed-settings inspection and editing command.
add_executable(umicom-studio-settings
    src/tools/settings_main.c
)
target_link_libraries(umicom-studio-settings PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-settings)
umicom_apply_sanitizers(umicom-studio-settings)

# -----------------------------------------------------------------------------
# GTK4 Studio product implementation
# -----------------------------------------------------------------------------
if(UMICOM_STUDIO_BUILD_GTK)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
    pkg_check_modules(GOBJECT REQUIRED IMPORTED_TARGET gobject-2.0)
    pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)
    pkg_check_modules(JSON_GLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
    pkg_check_modules(SOUP3 REQUIRED IMPORTED_TARGET libsoup-3.0)
    pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
    pkg_check_modules(PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
    pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
    pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)
    pkg_check_modules(HARFBUZZ REQUIRED IMPORTED_TARGET harfbuzz)
    pkg_check_modules(GRAPHENE REQUIRED IMPORTED_TARGET graphene-1.0)
    pkg_check_modules(SOURCEVIEW5 QUIET IMPORTED_TARGET gtksourceview-5)

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/StudioSources.cmake")

    add_library(umicom_studio_product STATIC
        ${UMICOM_STUDIO_PRODUCT_SOURCES}
    )
    add_library(Umicom::StudioProduct ALIAS umicom_studio_product)

    # These paths preserve the current product's include relationships.  Later
    # extraction commits will reduce this list as each bounded module receives
    # a public contract and its own CMake target.
    target_include_directories(umicom_studio_product PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/build/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/prefs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/includes"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/providers"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/chat/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/transpile"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/transpile/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/search"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/search/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/git/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/watchers/include"
    )

    target_compile_definitions(umicom_studio_product PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=13
        UMICOM_VERSION_PATCH=0
        gtk_widget_destroy=g_object_unref
    )

    target_link_libraries(umicom_studio_product
        PUBLIC
            Umicom::Framework
        PRIVATE
            PkgConfig::GLIB
            PkgConfig::GOBJECT
            PkgConfig::GIO
            PkgConfig::JSON_GLIB
            PkgConfig::SOUP3
            PkgConfig::CURL
            PkgConfig::GTK4
            PkgConfig::PIXBUF
            PkgConfig::CAIRO
            PkgConfig::PANGO
            PkgConfig::HARFBUZZ
            PkgConfig::GRAPHENE
    )

    if(TARGET PkgConfig::SOURCEVIEW5)
        target_link_libraries(umicom_studio_product PRIVATE
            PkgConfig::SOURCEVIEW5
        )
    endif()

    # The product implementation is not warning-clean yet.  Keep a practical
    # warning set here while new Framework and StudioCore code remains strict.
    if(MSVC)
        target_compile_options(umicom_studio_product PRIVATE /W4)
    else()
        target_compile_options(umicom_studio_product PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-parameter
        )
    endif()

    add_executable(umicom-studio-ide
        src/gtk/main.c
    )

    set(UMICOM_STUDIO_WINDOWS_RESOURCE
        "${CMAKE_SOURCE_DIR}/win/umicom.rc"
    )

    if(WIN32 AND EXISTS "${UMICOM_STUDIO_WINDOWS_RESOURCE}")
        target_sources(umicom-studio-ide PRIVATE
            "${UMICOM_STUDIO_WINDOWS_RESOURCE}"
        )
    endif()

    target_include_directories(umicom-studio-ide PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"
        "${CMAKE_SOURCE_DIR}/win"
    )

    target_link_libraries(umicom-studio-ide PRIVATE
        Umicom::StudioCore
        Umicom::StudioProduct
        PkgConfig::GTK4
        PkgConfig::GLIB
        PkgConfig::GIO
    )

    target_compile_definitions(umicom-studio-ide PRIVATE
        USIDE_GUI=$<IF:$<BOOL:${UMICOM_STUDIO_GUI}>,1,0>
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=13
        UMICOM_VERSION_PATCH=0
    )

    if(WIN32 AND UMICOM_STUDIO_GUI)
        set_property(TARGET umicom-studio-ide PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)
endif()

# -----------------------------------------------------------------------------
# Framework integration tests
# -----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-studio-framework-bootstrap-test
        tests/test_framework_bootstrap.c
    )
    target_link_libraries(umicom-studio-framework-bootstrap-test PRIVATE
        Umicom::StudioCore
    )
    umicom_apply_warnings(umicom-studio-framework-bootstrap-test)
    umicom_apply_sanitizers(umicom-studio-framework-bootstrap-test)

    add_test(
        NAME studio.framework_bootstrap
        COMMAND umicom-studio-framework-bootstrap-test
    )

    add_executable(umicom-studio-version-test
        tests/test_version.c
    )
    target_link_libraries(umicom-studio-version-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-version-test)
    add_test(NAME studio.version COMMAND umicom-studio-version-test)

    add_executable(umicom-studio-services-test
        tests/test_services.c
    )
    target_link_libraries(umicom-studio-services-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-services-test)
    add_test(NAME studio.services COMMAND umicom-studio-services-test)

    add_executable(umicom-studio-doctor-test
        tests/test_doctor.c
    )
    target_link_libraries(umicom-studio-doctor-test PRIVATE Umicom::StudioCore)
    target_compile_definitions(umicom-studio-doctor-test PRIVATE
        UMICOM_STUDIO_SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
    )
    umicom_apply_warnings(umicom-studio-doctor-test)
    add_test(NAME studio.doctor COMMAND umicom-studio-doctor-test)

    add_executable(umicom-studio-diagnostics-test
        tests/test_diagnostics.c
    )
    target_link_libraries(umicom-studio-diagnostics-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-diagnostics-test)
    umicom_apply_sanitizers(umicom-studio-diagnostics-test)
    add_test(NAME studio.diagnostics COMMAND umicom-studio-diagnostics-test)


    add_executable(umicom-studio-settings-test
        tests/test_settings.c
    )
    target_link_libraries(umicom-studio-settings-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-settings-test)
    umicom_apply_sanitizers(umicom-studio-settings-test)
    add_test(NAME studio.settings COMMAND umicom-studio-settings-test)
endif()

install(TARGETS
    umicom-studio-console
    umicom-studio-doctor
    umicom-studio-diagnostics
    umicom-studio-settings
    RUNTIME DESTINATION bin
)
