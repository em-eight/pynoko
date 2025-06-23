#pragma once

#include "PhongDrawable.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class DrawableSpheroid : public PhongDrawable {
public:
    DrawableSpheroid(float radiusX, float radiusY, float radiusZ, int slices=16, int stacks=16);
    virtual ~DrawableSpheroid() {}
    virtual void draw();

private:
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;

    std::vector<float> mVertices;
    std::vector<unsigned int> mIndices;
};

} // gfx
} // bolt