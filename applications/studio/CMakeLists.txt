#-----------------------------------------------------------------------------
# Umicom Studio IDE application targets
# File: applications/studio/CMakeLists.txt
#
# PURPOSE:
#   Create explicit Framework-based Studio targets while preserving the current
#   GTK4 code in a compatibility library.
#-----------------------------------------------------------------------------
set(UMICOM_STUDIO_VERSION "0.10.0")

# -----------------------------------------------------------------------------
# New Framework-based Studio composition root
# -----------------------------------------------------------------------------
add_library(umicom_studio_core STATIC
    src/core/bootstrap.c
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

# A small console executable verifies the Framework lifecycle without GTK.
add_executable(umicom-studio-console
    src/console/main.c
)
target_link_libraries(umicom-studio-console PRIVATE
    Umicom::StudioCore
)
umicom_apply_warnings(umicom-studio-console)
umicom_apply_sanitizers(umicom-studio-console)

# -----------------------------------------------------------------------------
# Preserved GTK4 implementation
# -----------------------------------------------------------------------------
if(UMICOM_STUDIO_BUILD_LEGACY_GTK)
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

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/LegacySources.cmake")

    add_library(umicom_studio_legacy STATIC
        ${UMICOM_STUDIO_LEGACY_SOURCES}
    )
    add_library(Umicom::StudioLegacy ALIAS umicom_studio_legacy)

    # These include paths reproduce the old build while the files are gradually
    # moved into proper Framework and Studio module targets.
    target_include_directories(umicom_studio_legacy PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/build/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/core/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/editor/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/chrome/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/dialogs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/llm_lab/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/menu/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/prefs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/theme"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/llm"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/llm/includes"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/llm/providers"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/panes/chat/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/panes/output/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/panes/problems/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/plugins/transpile"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/plugins/transpile/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/search"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/search/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/ui/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/ui/resources"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/util/fs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/util/git/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/util/log/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/util/sys/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/util/watchers/include"
    )

    target_compile_definitions(umicom_studio_legacy PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=10
        UMICOM_VERSION_PATCH=0
        gtk_widget_destroy=g_object_unref
    )

    target_link_libraries(umicom_studio_legacy
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
        target_link_libraries(umicom_studio_legacy PRIVATE
            PkgConfig::SOURCEVIEW5
        )
    endif()

    # The old source is not warning-clean yet.  New Framework-based code still
    # uses the stricter warning set from the root project.
    if(MSVC)
        target_compile_options(umicom_studio_legacy PRIVATE /W4)
    else()
        target_compile_options(umicom_studio_legacy PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-parameter
        )
    endif()

    add_executable(umicom-studio-ide
        src/gtk/main.c
    )

    # Keep Windows resources in their original root location during this first
    # migration slice.  Moving them now would add avoidable runtime risk.
    set(UMICOM_STUDIO_WINDOWS_RESOURCE
        "${CMAKE_SOURCE_DIR}/win/umicom.rc"
    )

    if(WIN32 AND EXISTS "${UMICOM_STUDIO_WINDOWS_RESOURCE}")
        target_sources(umicom-studio-ide PRIVATE
            "${UMICOM_STUDIO_WINDOWS_RESOURCE}"
        )
    endif()

    target_include_directories(umicom-studio-ide PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/legacy/ui/include"
        "${CMAKE_SOURCE_DIR}/win"
    )

    target_link_libraries(umicom-studio-ide PRIVATE
        Umicom::StudioCore
        Umicom::StudioLegacy
        PkgConfig::GTK4
        PkgConfig::GLIB
        PkgConfig::GIO
    )

    target_compile_definitions(umicom-studio-ide PRIVATE
        USIDE_GUI=$<IF:$<BOOL:${UMICOM_STUDIO_GUI}>,1,0>
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=10
        UMICOM_VERSION_PATCH=0
    )

    if(WIN32 AND UMICOM_STUDIO_GUI)
        set_property(TARGET umicom-studio-ide PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)
endif()

# -----------------------------------------------------------------------------
# Tests
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
endif()

install(TARGETS umicom-studio-console RUNTIME DESTINATION bin)
