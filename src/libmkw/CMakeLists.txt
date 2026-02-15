add_subdirectory(bolt EXCLUDE_FROM_ALL)
add_subdirectory(extern/glfw EXCLUDE_FROM_ALL)

add_library(libmkw
    KclOpengl.cpp
    MkwVis.cpp
    RaceCamera.cpp
)
target_link_libraries(libmkw PUBLIC bolt::gfx bolt::math glfw libkinoko)
target_compile_definitions(libmkw PRIVATE LIBMKW_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
