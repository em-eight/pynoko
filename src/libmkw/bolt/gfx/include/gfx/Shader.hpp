#pragma once

#include "math/Vector.hpp"
#include "math/Matrix.hpp"
#include "Color.hpp"

#include "gl_defines.h"

namespace bolt {
namespace gfx {

/**
 * Utility class for compiling and managing shaders
 */
class Shader {
public:
    // creates uninitialized shader
    Shader() = default;
    // compiles and loads shader from vertex and fragment components
    Shader(const char* vtxShader, const char* fragShader);

    /// @beginGetters
    unsigned int id() const { return mId; }
    /// @endGetters

    // activate shader
    void use() {
        glUseProgram(mId);
    }

    /**
     * \name Functions for setting shader uniforms
     * \{
     */
    void setVec4(const char* name, const math::Vector4f& value) {
        glUniform4fv(glGetUniformLocation(mId, name), 1, (const GLfloat*)&value);
    }

    void setColor(const char* name, const Color& value) {
        glUniform4fv(glGetUniformLocation(mId, name), 1, (const GLfloat*)&value);
    }

    // set matrix row-major (i.e. gets transposed in opengl)
    void setMat4(const char* name, const math::Matrix44f& value) {
        glUniformMatrix4fv(glGetUniformLocation(mId, name), 1, GL_TRUE, (const GLfloat*)&value);
    }
    /** \} */

private:
    unsigned int mId;
};

} // gfx
} // bolt
