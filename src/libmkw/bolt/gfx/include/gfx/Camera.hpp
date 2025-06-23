#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"
#include "math/Quat.hpp"

namespace bolt {
namespace gfx {

class Camera {
public:
    Camera();
    virtual ~Camera() {}
    /// send view and projection matrices to GPU
    void onDraw();

    // controls
    virtual void onScroll(float amount) {}
    virtual void onDrag(float x, float y) {}
    virtual void setAspectRatio(float aspectRatio) {}

    math::Matrix44f& getView() { return mView; }
    math::Matrix44f& getProjection() { return mProjection; }

protected:
    unsigned int UBO;
    math::Matrix44f mView;
    math::Matrix44f mProjection;
};

class OrbitCamera : public Camera {
public:
    OrbitCamera();
    void setPos(const math::Vector3f& pos);
    void setRot(const math::Quatf& pos);
    void setFocus(const math::Vector3f& focus);
    virtual void onScroll(float amount) override;
    virtual void onDrag(float x, float y) override;

    void setAspectRatio(float aspectRatio) override { mAspectRatio = aspectRatio; updatePerspectiveMat(); }

private:
    void updatePerspectiveMat() { mProjection.setPerspective(mFovy, mAspectRatio, 0.1, 100); }

    math::Vector3f mPos;
    math::Vector3f mFocus;
    float mFovy;
    float mAspectRatio;
};

} // gfx
} // bolt
