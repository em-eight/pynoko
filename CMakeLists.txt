cmake_minimum_required(VERSION 3.18)

# We are building a shared object, so all subdirs should build with PIC in mind
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Function: pick GCC or Clang, ignore MSVC
function(pick_gcc_or_clang out_c out_cxx)
    # Detect GCC
    find_program(GCC gcc)
    find_program(GPP g++)
    if(GCC AND GPP)
        message(STATUS "Using GCC: ${GCC}, ${GPP}")
        set(${out_c} "${GCC}" PARENT_SCOPE)
        set(${out_cxx} "${GPP}" PARENT_SCOPE)
        return()
    endif()

    # Detect Clang
    find_program(CLANG clang)
    find_program(CLANGXX clang++)
    if(CLANG AND CLANGXX)
        message(STATUS "Using Clang: ${CLANG}, ${CLANGXX}")
        set(${out_c} "${CLANG}" PARENT_SCOPE)
        set(${out_cxx} "${CLANGXX}" PARENT_SCOPE)
        return()
    endif()

    # No suitable compiler found
    message(FATAL_ERROR "No supported compiler found. Only GCC or Clang are supported. Don't even think about using MSVC")
endfunction()

# Pick compiler before project()
pick_gcc_or_clang(C_COMPILER CXX_COMPILER)

set(CMAKE_C_COMPILER ${C_COMPILER} CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER ${CXX_COMPILER} CACHE STRING "C++ compiler" FORCE)

project(pynoko VERSION 1.0 LANGUAGES C CXX)

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
