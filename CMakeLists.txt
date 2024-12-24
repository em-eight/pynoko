cmake_minimum_required(VERSION 3.18)
project(mkw_rl VERSION 1.0)

include(cmake/CPM.cmake)

CPMAddPackage("gh:em-eight/Kinoko#3a378e3")

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
add_subdirectory(extern/nanobind)

nanobind_add_module(pynoko nanobind.cpp)
target_link_libraries(pynoko PRIVATE libkinoko)
