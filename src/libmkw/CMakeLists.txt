set(BOLT_BUILD_EXAMPLES OFF)
add_subdirectory(extern/bolt EXCLUDE_FROM_ALL)

csp_add_program(kcl
    VERT ${CMAKE_CURRENT_SOURCE_DIR}/share/kcl.vert
    FRAG ${CMAKE_CURRENT_SOURCE_DIR}/share/kcl.frag
)

add_library(libmkw
    KclDrawable.cpp
    MkwVis.cpp
    RaceCamera.cpp
)
target_link_libraries(libmkw PUBLIC bolt::gfx bolt::math libkinoko)
csp_shader_dependency(libmkw PRIVATE kcl)
