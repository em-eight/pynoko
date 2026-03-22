#include "RaceCamera.hpp"

#include <cmath>

#define DEG2RAD(deg) ((deg) * (M_PI / 180.0))

using namespace bolt::math;
using namespace Kinoko;

RaceCamera::RaceCamera() : mFovy(DEG2RAD(60)), mAspectRatio(1) {
    updatePerspectiveMat();
    mMtx.setRotation(0, 0, 0);
    mMtx.setTranslation(0, 0, -3);
}

void RaceCamera::setPos(const EGG::Vector3f& pos) {
    mMtx.setTranslation(pos.x, pos.y, pos.z);
    mWorldDirty = true;
}

void RaceCamera::setRot(const EGG::Quatf& rot) {
    float qw = rot.w;
    float qx = rot.v.x;
    float qy = rot.v.y;
    float qz = rot.v.z;

    mMtx(0,0) = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
    mMtx(0,1) = 2.0f*qx*qy - 2.0f*qz*qw;
    mMtx(0,2) = 2.0f*qx*qz + 2.0f*qy*qw;

    mMtx(1,0) = 2.0f*qx*qy + 2.0f*qz*qw;
    mMtx(1,1) = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
    mMtx(1,2) = 2.0f*qy*qz - 2.0f*qx*qw;

    mMtx(2,0) = 2.0f*qx*qz - 2.0f*qy*qw;
    mMtx(2,1) = 2.0f*qy*qz + 2.0f*qx*qw;
    mMtx(2,2) = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;

    Matrix44f y180Turn;
    y180Turn.setIdentity();
    y180Turn(0, 0) = -1;
    y180Turn(2, 2) = -1;
    mMtx = mMtx * y180Turn;

    mWorldDirty = true;
}
