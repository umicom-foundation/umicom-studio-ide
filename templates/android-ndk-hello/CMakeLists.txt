cmake_minimum_required(VERSION 3.22)
project(ndk_hello C)
set(CMAKE_C_STANDARD 17)
add_library(hello SHARED native-lib.c)
