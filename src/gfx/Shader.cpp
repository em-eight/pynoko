#include "Shader.hpp"

#include <filesystem>
#include <fstream>
#include <cstdio>

#define TOSTRINGIMPL(x) #x
#define TOSTRING(x) TOSTRINGIMPL(x)

#ifdef __FILE_NAME__
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define RUNTIME_ASSERT(e, m) \
    do { \
        if (!(e)) { \
            printf("[" FILE_NAME ":" TOSTRING(__LINE__) "] ASSERT: " #e ", MSG: %s\n", m); \
            abort(); \
        } \
    } while (0)

using namespace std;

void* loadFile(const filesystem::path& fullpath, unsigned int& size) {
    std::ifstream file(fullpath, std::ios::binary | std::ios::ate);
    RUNTIME_ASSERT(!!file, ("Could not open file " + fullpath.string()).c_str());

    // Get file size
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate buffer and read the file
    void* buffer = malloc(size);
    file.read((char*)buffer, size);
    RUNTIME_ASSERT(!!file, ("Error reading file " + fullpath.string()).c_str());

    return buffer;
}

void* loadResource(const char* path, unsigned int& size) {
    filesystem::path root = "./res";
    filesystem::path fullpath = root / path;

    return loadFile(fullpath, size);
}

namespace gfx {

void loadShader(const char* name, unsigned int& shader, GLenum type) {
    unsigned int size;
    void* source = loadResource(name, size);

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
    //assertProgramOk(mId);
    //glCheckError();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

} // gfx
