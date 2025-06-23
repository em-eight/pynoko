#pragma once

#include "PhongDrawable.hpp"

namespace bolt {
namespace gfx {

class DrawableCuboid : public PhongDrawable {
public:
    DrawableCuboid(float sizeX, float sizeY, float sizeZ);
    virtual ~DrawableCuboid() {}
    virtual void draw();

private:
    unsigned int VBO;
    unsigned int VAO;
};

} // gfx
} // bolt