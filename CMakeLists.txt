#-----------------------------------------------------------------------------
# Umicom Studio IDE
# File: CMakeLists.txt
#
# PURPOSE:
#   Build Umicom Framework first, then build Umicom Studio as a Framework
#   application.  This replaces the old single-target recursive source glob.
#
# MIGRATION STAGE:
#   The existing GTK4 source remains under applications/studio/src/legacy.
#   The new Studio bootstrap starts the Framework Master Controller around the
#   preserved GUI.  Later commits replace one legacy service at a time.
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
    VERSION 0.10.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(UMICOM_ENABLE_STRICT_WARNINGS
       "Enable strict compiler warnings for new Framework-based code"
       ON)
option(UMICOM_ENABLE_SANITIZERS
       "Enable ASan and UBSan where supported"
       OFF)
option(UMICOM_STUDIO_BUILD_LEGACY_GTK
       "Build the preserved GTK4 Studio implementation"
       ON)
option(UMICOM_STUDIO_GUI
       "Build the Windows GUI-subsystem Studio executable"
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

# Framework resolution order:
#   1. A suite build has already created Umicom::Framework.
#   2. This standalone Studio checkout contains framework/CMakeLists.txt.
#   3. An installed Framework SDK is available through find_package().
if(NOT TARGET Umicom::Framework)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/framework/CMakeLists.txt")
        add_subdirectory(framework)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/framework/README.md")
        message(FATAL_ERROR
            "The framework submodule exists but still contains only the initial skeleton. "
            "Complete Umicom Framework Foundation 0.4.1 first, update the submodule, "
            "and then configure Studio again."
        )
    else()
        find_package(UmicomFramework CONFIG REQUIRED)
    endif()
endif()

add_subdirectory(applications/studio)

message(STATUS "Umicom Studio IDE version: ${PROJECT_VERSION}")
message(STATUS "Framework target available: $<TARGET_EXISTS:Umicom::Framework>")
message(STATUS "Legacy GTK build: ${UMICOM_STUDIO_BUILD_LEGACY_GTK}")