# -----------------------------------------------------------------------------
# Umicom Studio IDE
# PURPOSE: Clean, reproducible build for MSYS2/MinGW (Windows) and Linux.
#          Fixes prior issues where include directories were incorrectly
#          added as -D macros (producing errors like 'macro names must be
#          identifiers'). Also selects Console vs GUI subsystem sanely.
# Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
# Last updated: 2025-10-11
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.22)
project(ndk_hello C)
set(CMAKE_C_STANDARD 17)
add_library(hello SHARED native-lib.c)
