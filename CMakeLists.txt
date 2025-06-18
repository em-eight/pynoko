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

set(KINOKO_USE_RTTI ON)
CPMAddPackage("gh:em-eight/Kinoko#a3e1e73")

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
add_subdirectory(extern/nanobind)

find_package(glfw3 REQUIRED)

set(PYNOKO_SOURCES
    src/nanobind.cpp

    src/gfx/KclOpengl.cpp
    src/gfx/MkwVis.cpp
    src/gfx/Shader.cpp
    src/gfx/Camera.cpp
    src/gfx/Color.cpp
    src/gfx/glad/glad.c

    src/math/Matrix.cpp

    src/util/Filesystem.cpp
)

nanobind_add_module(pynoko ${PYNOKO_SOURCES})
target_include_directories(pynoko
    PRIVATE
    src
)
target_link_libraries(pynoko PRIVATE libkinoko glfw)
