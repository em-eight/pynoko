#pragma once

#include "gfx/Camera.hpp"

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>

class RaceCamera : public bolt::gfx::Camera {
public:
    RaceCamera();

    void setAspectRatio(float aspectRatio) override { mAspectRatio = aspectRatio; updatePerspectiveMat(); }
    void setPos(const EGG::Vector3f& pos);
    void setRot(const EGG::Quatf& pos);

private:
    void updatePerspectiveMat() { mProjection.setPerspective(mFovy, mAspectRatio, 1, 25000); }

    float mFovy;
    float mAspectRatio;
};
