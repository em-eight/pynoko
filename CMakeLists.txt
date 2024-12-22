cmake_minimum_required(VERSION 3.12)
project(mkw_rl VERSION 1.0)

include(cmake/CPM.cmake)

CPMAddPackage("gh:em-eight/Kinoko#c084989")

find_package(Python 3.12 COMPONENTS Interpreter Development)
add_subdirectory(extern/pybind11)

pybind11_add_module(pynoko main.cpp)
target_link_libraries(pynoko PRIVATE libkinoko)
