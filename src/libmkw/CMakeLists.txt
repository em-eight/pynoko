add_subdirectory(bolt EXCLUDE_FROM_ALL)
add_subdirectory(extern/glfw EXCLUDE_FROM_ALL)

find_package(glfw3 REQUIRED)

add_library(libmkw
    KclOpengl.cpp
    MkwVis.cpp
    RaceCamera.cpp
)
target_link_libraries(libmkw PRIVATE libkinoko glfw PUBLIC bolt::gfx bolt::math)
target_compile_definitions(libmkw PRIVATE LIBMKW_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
