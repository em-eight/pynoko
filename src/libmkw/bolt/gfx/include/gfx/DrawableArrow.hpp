#pragma once

#include "PhongDrawable.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class DrawableArrow3d : public PhongDrawable {
public:
    DrawableArrow3d(float shaftLength, float tipLength, float tipDiameter, unsigned int samples = 10);
    virtual ~DrawableArrow3d() {}
    virtual void draw();

private:
    void generateVertices(float shaftLength, float tipLength, float tipDiameter, unsigned int samples);

    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

} // gfx
} // bolt