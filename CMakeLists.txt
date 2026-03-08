cmake_minimum_required(VERSION 3.18)

# We are building a shared object, so all subdirs should build with PIC in mind
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Function: pick GCC or Clang, ignore MSVC
function(pick_gcc_or_clang out_c out_cxx)
    # Detect Clang
    find_program(CLANG clang)
    find_program(CLANGXX clang++)
    if(CLANG AND CLANGXX)
        message(STATUS "Using Clang: ${CLANG}, ${CLANGXX}")
        set(${out_c} "${CLANG}" PARENT_SCOPE)
        set(${out_cxx} "${CLANGXX}" PARENT_SCOPE)
        return()
    endif()

    # Detect GCC
    find_program(GCC gcc)
    find_program(GPP g++)
    if(GCC AND GPP)
        message(STATUS "Using GCC: ${GCC}, ${GPP}")
        set(${out_c} "${GCC}" PARENT_SCOPE)
        set(${out_cxx} "${GPP}" PARENT_SCOPE)
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

option(PYNOKO_BUILD_GUI "Build with GUI (glfw) support" ON)

set(KINOKO_USE_RTTI ON)
CPMAddPackage("gh:em-eight/Kinoko#9d1cff7")

if (NOT SKBUILD)
    # local build
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    add_subdirectory(extern/nanobind EXCLUDE_FROM_ALL)
else()
    # scikit-build (wheel build)
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module OPTIONAL_COMPONENTS Development.SABIModule)
    find_package(nanobind CONFIG REQUIRED)
endif()

if (PYNOKO_BUILD_GUI)
    add_subdirectory(src/libmkw EXCLUDE_FROM_ALL)
endif()

set(PYNOKO_SOURCES
    src/nanobind.cpp
)

if (NOT SKBUILD)
    set(PYNOKO_MODULE_NAME pynoko)
else()
    set(PYNOKO_MODULE_NAME _pynoko_impl)
endif()

nanobind_add_module(
    ${PYNOKO_MODULE_NAME}
    STABLE_ABI
    src/nanobind.cpp
)
target_include_directories(${PYNOKO_MODULE_NAME}
    PRIVATE
    src
)
target_link_libraries(${PYNOKO_MODULE_NAME} PRIVATE libkinoko)
if(PYNOKO_BUILD_GUI)
    target_link_libraries(${PYNOKO_MODULE_NAME} PRIVATE libmkw)
    target_compile_definitions(${PYNOKO_MODULE_NAME} PRIVATE PYNOKO_BUILD_GUI)
endif()
target_compile_definitions(${PYNOKO_MODULE_NAME} PRIVATE PYNOKO_MODULE_NAME=${PYNOKO_MODULE_NAME})

if (SKBUILD)
    # installation directives for python wheels
    if(UNIX AND NOT APPLE)
        set_target_properties(${PYNOKO_MODULE_NAME} PROPERTIES
            INSTALL_RPATH "$ORIGIN"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif()
    install(TARGETS ${PYNOKO_MODULE_NAME} LIBRARY DESTINATION pynoko)
    install(TARGETS libkinoko LIBRARY DESTINATION pynoko)
endif()
