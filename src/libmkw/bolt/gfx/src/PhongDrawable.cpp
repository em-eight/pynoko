#include "gfx/PhongDrawable.hpp"

#include "gfx/Shader.hpp"
#include "gfx/common.h"

namespace bolt {
namespace gfx {

PhongDrawable::PhongDrawable() {
    mShader = Shader(BOLT_GFX_RES("phong.vert"), BOLT_GFX_RES("phong.frag"));
}

void PhongDrawable::onDraw() {
    this->Drawable3d::onDraw();

    mShader.use();
    mShader.setColor("ambientColor", mAmbient);
    // Future: diffuse, maybe specular
}

} // gfx
} // bolt
