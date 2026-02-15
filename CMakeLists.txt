cmake_minimum_required(VERSION 3.18)

# We are building a shared object, so all subdirs should build with PIC in mind
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(pynoko VERSION 1.0)

include(cmake/CPM.cmake)
include(GNUInstallDirs)

set(KINOKO_USE_RTTI ON)
CPMAddPackage("gh:em-eight/Kinoko#867dc3f")

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
