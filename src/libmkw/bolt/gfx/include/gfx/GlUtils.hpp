#pragma once

#include "util/Filesystem.hpp"
#include "util/common.h"

#include "gl_defines.h"

#include <cstdlib>

namespace bolt {
namespace gfx {

inline void assertProgramOk(unsigned int program) {
#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        RUNTIME_ASSERT(success, infoLog);
    }
#endif
}

void openglDebugOutputCallback(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);

} // gfx
} // bolt
