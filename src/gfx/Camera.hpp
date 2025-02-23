#pragma once

#include "math/Matrix.hpp"

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>

namespace gfx {

class Camera {
public:
    Camera(int width, int height);
    virtual ~Camera() {}
    /// send view and projection matrices to GPU
    void onDraw();

    void setPos(const EGG::Vector3f& pos);
    void setRot(const EGG::Quatf& pos);

    math::Matrix44f& getView() { return mView; }
    math::Matrix44f& getProjection() { return mProjection; }

protected:
    int mWidth;
    int mHeight;
    unsigned int UBO;
    math::Matrix44f mView;
    math::Matrix44f mProjection;
    EGG::Quatf mRot;
    EGG::Vector3f mPos;
};
} // gfx
