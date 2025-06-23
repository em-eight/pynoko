#include "gfx/Shader.hpp"
#include "gfx/GlUtils.hpp"

namespace bolt {
namespace gfx {

void loadShader(const char* name, unsigned int& shader, GLenum type) {
    unsigned int size;
    void* source = util::Filesystem::loadFile(name, size);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar* const*)&source, (const GLint*)&size);
    glCompileShader(shader);

#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        printf("Shader Source (%s):\n%s\n", name, static_cast<const char*>(source));

        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        RUNTIME_ASSERT(success, infoLog);
    }
#endif

    free(source);
}

Shader::Shader(const char* vtxShader, const char* fragShader) {
    unsigned int vertexShader, fragmentShader;
    loadShader(vtxShader, vertexShader, GL_VERTEX_SHADER);
    loadShader(fragShader, fragmentShader, GL_FRAGMENT_SHADER);

    mId = glCreateProgram();
    glAttachShader(mId, vertexShader);
    glAttachShader(mId, fragmentShader);
    glLinkProgram(mId);
    assertProgramOk(mId);
    glCheckError();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

} // gfx
} // bolt
