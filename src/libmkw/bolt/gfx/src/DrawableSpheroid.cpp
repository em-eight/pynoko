#include "gfx/DrawableSpheroid.hpp"
#include "gfx/GlUtils.hpp"
#include "gfx/gl_defines.h"

#include "math/math.h"

namespace bolt {
namespace gfx {

static std::vector<float> generateSphereVertices(float radiusX, float radiusY, float radiusZ, int slices, int stacks) {
    std::vector<float> vertices;

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            float x = radiusX * sinPhi * cosTheta;
            float y = radiusY * cosPhi;
            float z = radiusZ * sinPhi * sinTheta;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            math::Vector3f normal(x, y, z);
            normal.normalize();
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }
    return vertices;
}

static std::vector<unsigned int> generateSphereIndices(int slices, int stacks) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    return indices;
}

DrawableSpheroid::DrawableSpheroid(float rX, float rY, float rZ, int slices, int stacks) {
    mVertices = generateSphereVertices(rX, rY, rZ, slices, stacks);
    mIndices = generateSphereIndices(slices, stacks);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

void DrawableSpheroid::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
    glBindVertexArray(0);
    glUseProgram(0);
}

} // gfx
} // bolt
