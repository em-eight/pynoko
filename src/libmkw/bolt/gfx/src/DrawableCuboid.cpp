#include "gfx/DrawableCuboid.hpp"
#include "gfx/GlUtils.hpp"
#include "gfx/gl_defines.h"

#include "math/math.h"

namespace bolt {
namespace gfx {

DrawableCuboid::DrawableCuboid(float sizeX, float sizeY, float sizeZ) {
    float hx = sizeX / 2;
    float hy = sizeY / 2;
    float hz = sizeZ / 2;

    float vertices[] = {
        -hx, -hy,  hz, 0.0f, 0.0f, 1.0f,
         hx, -hy,  hz, 0.0f, 0.0f, 1.0f,
         hx,  hy,  hz, 0.0f, 0.0f, 1.0f,
        -hx, -hy,  hz, 0.0f, 0.0f, 1.0f,
        -hx,  hy,  hz, 0.0f, 0.0f, 1.0f,
         hx,  hy,  hz, 0.0f, 0.0f, 1.0f,

        -hx, -hy, -hz, 0.0f, 0.0f, -1.0f,
         hx, -hy, -hz, 0.0f, 0.0f, -1.0f,
         hx,  hy, -hz, 0.0f, 0.0f, -1.0f,
        -hx, -hy, -hz, 0.0f, 0.0f, -1.0f,
        -hx,  hy, -hz, 0.0f, 0.0f, -1.0f,
         hx,  hy, -hz, 0.0f, 0.0f, -1.0f,

        -hx,  hy, -hz, 0.0f, 1.0f, 0.0f,
         hx,  hy, -hz, 0.0f, 1.0f, 0.0f,
         hx,  hy,  hz, 0.0f, 1.0f, 0.0f,
        -hx,  hy, -hz, 0.0f, 1.0f, 0.0f,
        -hx,  hy,  hz, 0.0f, 1.0f, 0.0f,
         hx,  hy,  hz, 0.0f, 1.0f, 0.0f,

        -hx, -hy, -hz, 0.0f, -1.0f, 0.0f,
         hx, -hy, -hz, 0.0f, -1.0f, 0.0f,
         hx, -hy,  hz, 0.0f, -1.0f, 0.0f,
        -hx, -hy, -hz, 0.0f, -1.0f, 0.0f,
        -hx, -hy,  hz, 0.0f, -1.0f, 0.0f,
         hx, -hy,  hz, 0.0f, -1.0f, 0.0f,

         hx, -hy, -hz, 1.0f, 0.0f, 0.0f,
         hx,  hy, -hz, 1.0f, 0.0f, 0.0f,
         hx,  hy,  hz, 1.0f, 0.0f, 0.0f,
         hx, -hy, -hz, 1.0f, 0.0f, 0.0f,
         hx, -hy,  hz, 1.0f, 0.0f, 0.0f,
         hx,  hy,  hz, 1.0f, 0.0f, 0.0f,

        -hx, -hy, -hz, -1.0f, 0.0f, 0.0f,
        -hx,  hy, -hz, -1.0f, 0.0f, 0.0f,
        -hx,  hy,  hz, -1.0f, 0.0f, 0.0f,
        -hx, -hy, -hz, -1.0f, 0.0f, 0.0f,
        -hx, -hy,  hz, -1.0f, 0.0f, 0.0f,
        -hx,  hy,  hz, -1.0f, 0.0f, 0.0f,
    };

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

void DrawableCuboid::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6);
    glBindVertexArray(0);
    glUseProgram(0);
}

} // gfx
} // bolt
