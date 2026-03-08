cmake_minimum_required(VERSION 3.18)

# We are building a shared object, so all subdirs should build with PIC in mind
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(pynoko VERSION 1.0 LANGUAGES C CXX)

# Define supported compilers
set(SUPPORTED_COMPILERS "GNU" "Clang" "AppleClang")
if(NOT CMAKE_CXX_COMPILER_ID IN_LIST SUPPORTED_COMPILERS)
    message(FATAL_ERROR
        "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}\n"
        "This project requires GCC or Clang.\n")
endif()

include(cmake/CPM.cmake)
include(GNUInstallDirs)

option(PYNOKO_BUILD_GUI "Build with GUI (glfw) support" ON)

set(KINOKO_USE_RTTI ON)
CPMAddPackage("gh:em-eight/Kinoko#158386a")

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
