cmake_minimum_required(VERSION 3.18)

# Force Clang compilers
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# We are building a shared object, so all subdirs should build with PIC in mind
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(pynoko VERSION 1.0)

# Verify the compiler is Clang
if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project must be built with Clang!")
endif()
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project must be built with Clang++!")
endif()

include(cmake/CPM.cmake)
include(GNUInstallDirs)

set(KINOKO_USE_RTTI ON)
CPMAddPackage("gh:em-eight/Kinoko#53d0116")

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
add_subdirectory(extern/nanobind)
add_subdirectory(src/libmkw)

set(PYNOKO_SOURCES
    src/nanobind.cpp
)

nanobind_add_module(pynoko src/nanobind.cpp)
target_include_directories(pynoko
    PRIVATE
    src
)
target_link_libraries(pynoko PRIVATE libkinoko libmkw)
