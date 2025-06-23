#pragma once

#include "gfx/Camera.hpp"

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>

class RaceCamera : public bolt::gfx::Camera {
public:
    RaceCamera(float aspecRatio);
    void onDraw();
    void setPos(const EGG::Vector3f& pos);
    void setRot(const EGG::Quatf& pos);

private:
    EGG::Quatf mRot;
    EGG::Vector3f mPos;
};
