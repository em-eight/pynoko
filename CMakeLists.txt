cmake_minimum_required(VERSION 3.18)

# Force Clang compilers
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(mkw_rl VERSION 1.0)

# Verify the compiler is Clang
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project must be built with Clang!")
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project must be built with Clang++!")
endif()

include(cmake/CPM.cmake)

CPMAddPackage("gh:em-eight/Kinoko#b16e187")

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
add_subdirectory(extern/nanobind)

nanobind_add_module(pynoko nanobind.cpp)
target_link_libraries(pynoko PRIVATE libkinoko)
