#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
#
# PURPOSE:
#   Build Umicom Framework first, then build Umicom Studio IDE as an independent
#   Framework application.  The Studio product source is organised below
#   applications/studio and is exposed through explicit CMake targets.
#
# ARCHITECTURE:
#   - Umicom::Framework owns reusable runtime and platform capabilities.
#   - Umicom::StudioCore owns the Studio composition root.
#   - Umicom::StudioProduct owns the current Studio product implementation.
#   - umicom-studio-console and umicom-studio-ide are independent frontends.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

project(
    UmicomStudioIDE
    VERSION 0.23.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(UMICOM_ENABLE_STRICT_WARNINGS
       "Enable strict compiler warnings for Framework-based code"
       ON)
option(UMICOM_ENABLE_SANITIZERS
       "Enable ASan and UBSan where supported"
       OFF)
option(UMICOM_STUDIO_BUILD_GTK
       "Build the GTK4 Umicom Studio IDE application"
       ON)
option(UMICOM_STUDIO_GUI
       "Build the Windows GUI-subsystem Studio executable"
       ON)
option(UMICOM_BUILD_TRADER_FOUNDATION
       "Build the independent Umicom Trader headless foundation application"
       ON)

include(CTest)

function(umicom_apply_warnings target)
    if(NOT UMICOM_ENABLE_STRICT_WARNINGS)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
        )
    endif()
endfunction()

function(umicom_apply_sanitizers target)
    if(UMICOM_ENABLE_SANITIZERS AND NOT MSVC)
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )
    endif()
endfunction()

# The Studio GTK4 build consumes the Framework GTK4 reference adapter.
if(UMICOM_STUDIO_BUILD_GTK)
    set(UMICOM_BUILD_GTK4_ADAPTER ON CACHE BOOL
        "Build the Framework GTK4 adapter required by Studio" FORCE)
endif()

# Framework resolution order:
#   1. A suite or superbuild has already created Umicom::Framework.
#   2. This standalone Studio checkout contains framework/CMakeLists.txt.
#   3. An installed Framework SDK is available through find_package().
if(NOT TARGET Umicom::Framework)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/framework/CMakeLists.txt")
        add_subdirectory(framework)
    else()
        find_package(UmicomFramework CONFIG REQUIRED)
    endif()
endif()

add_subdirectory(applications/studio)

# Studio Application Hub / Runtime Manager.
#
# This is deliberately a separate bounded Studio feature target.  It consumes
# the public Framework integration runtime introduced in Batch 18 without
# changing Framework or reaching into another Umicom application's private
# source tree.
add_subdirectory(applications/studio/runtime-manager)

# Studio workspace/editor productivity.
#
# This bounded feature owns document/session/navigation/search models and a
# GtkSourceView workbench without replacing the mature Studio product editor
# until the new behaviour has been exercised independently.
add_subdirectory(applications/studio/workspace-productivity)

# Trader is a separate application composition over the same public Framework
# SDK.  Keeping it in its own application directory makes later extraction to
# a dedicated repository mechanical rather than architectural.
if(UMICOM_BUILD_TRADER_FOUNDATION)
    add_subdirectory(applications/trader)
endif()

# Install a product licence and generate portable archives on every platform.
# Windows builds may additionally create an NSIS installer when NSIS is present.
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    DESTINATION share/licenses/umicom-studio)

set(CPACK_PACKAGE_NAME "Umicom Studio")
set(CPACK_PACKAGE_VENDOR "Umicom Foundation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Professional C23 application development and trading studio")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Umicom Studio")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_FILE_NAME "umicom-studio-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Umicom Studio")
    set(CPACK_NSIS_PACKAGE_NAME "Umicom Studio")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH OFF)
else()
    set(CPACK_GENERATOR "TGZ;ZIP")
endif()
include(CPack)

message(STATUS "Umicom Studio IDE version: ${PROJECT_VERSION}")
message(STATUS "GTK application build: ${UMICOM_STUDIO_BUILD_GTK}")
message(STATUS "C standard: C${CMAKE_C_STANDARD}")
