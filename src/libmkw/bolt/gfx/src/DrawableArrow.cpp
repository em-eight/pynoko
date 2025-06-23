#include "gfx/DrawableArrow.hpp"
#include "gfx/GlUtils.hpp"
#include "gfx/gl_defines.h"
#include "math/math.h"

#include <cmath>

namespace bolt {
namespace gfx {

DrawableArrow3d::DrawableArrow3d(float shaftLength, float tipLength, float tipDiameter, unsigned int samples) {
    // Generate vertices and indices for the arrow
    generateVertices(shaftLength, tipLength, tipDiameter, samples);

    // Generate and bind buffers
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

void DrawableArrow3d::generateVertices(float shaftLength, float tipLength, float tipDiameter, unsigned int samples) {
    // Shaft vertices
    vertices = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, shaftLength, 0.0f, 0.0f, 1.0f
    };

    // Tip base center
    float tipBaseZ = shaftLength - tipLength;
    float tipApexZ = shaftLength;

    // Generate cone base vertices
    for (unsigned int i = 0; i < samples; ++i) {
        float angle = (2.0f * M_PI * i) / samples;
        float x = (tipDiameter / 2.0f) * cos(angle);
        float y = (tipDiameter / 2.0f) * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(tipBaseZ);
        // approximate normal
        math::Vector3f coneNormal = math::Vector3f(x, y, tipLength).normalize();
        vertices.push_back(coneNormal.x);
        vertices.push_back(coneNormal.y);
        vertices.push_back(coneNormal.z);
    }

    // Add apex of the cone
    unsigned int tipApexIndex = vertices.size() / 6;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(tipApexZ);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);

    // Cone base center
    unsigned int tipBaseCenterIndex = vertices.size() / 6;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(tipBaseZ);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);

    // Indices for the cone base (GL_TRIANGLE_FAN)
    unsigned int sampleIndex = 2;
    // arrow edge cone
    indices.push_back(tipApexIndex);

    for (unsigned int i = 0; i < samples+1; ++i) {
        unsigned int next = i % samples;
        indices.push_back(sampleIndex + next);
    }

    // cone "cap"
    indices.push_back(tipBaseCenterIndex);

    for (unsigned int i = 0; i < samples+1; ++i) {
        unsigned int next = i % samples;
        indices.push_back(sampleIndex + next);
    }
}

void DrawableArrow3d::draw() {
    glBindVertexArray(VAO);

    // Draw shaft
    glDrawArrays(GL_LINES, 0, 2);

    // Draw cone
    int fanSize = indices.size() / 2;
    glDrawElements(GL_TRIANGLE_FAN, fanSize, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
    glDrawElements(GL_TRIANGLE_FAN, fanSize, GL_UNSIGNED_INT, (void*)(fanSize * sizeof(unsigned int)));

    glBindVertexArray(0);
    glUseProgram(0);
}

} // gfx
} // bolt
