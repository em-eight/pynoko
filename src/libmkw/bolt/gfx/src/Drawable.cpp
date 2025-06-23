#include "gfx/Drawable.hpp"

#include "gfx/gl_defines.h"

namespace bolt {
namespace gfx {

Drawable3d::Drawable3d() {
    mMtx.makeHomogeneous();
}

void Drawable3d::onDraw() {
    mShader.use();
    mShader.setMat4("model", mMtx);
}

void Drawable3d::setMtx(const math::Matrix34f& mtx) {
    mMtx(0,0) = mtx(0,0);
    mMtx(0,1) = mtx(0,1);
    mMtx(0,2) = mtx(0,2);
    mMtx(0,3) = mtx(0,3);
    mMtx(1,0) = mtx(1,0);
    mMtx(1,1) = mtx(1,1);
    mMtx(1,2) = mtx(1,2);
    mMtx(1,3) = mtx(1,3);
    mMtx(2,0) = mtx(2,0);
    mMtx(2,1) = mtx(2,1);
    mMtx(2,2) = mtx(2,2);
    mMtx(2,3) = mtx(2,3);
}

} // gfx
} // bolt
