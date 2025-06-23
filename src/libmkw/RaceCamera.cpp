#include "RaceCamera.hpp"

#include "glad/glad.h"

#include <cmath>

#define DEG2RAD(deg) ((deg) * (M_PI / 180.0))

using namespace bolt::math;

struct CameraData {
    Matrix44f projection;
    Matrix44f view;
    EGG::Quatf rot;
    EGG::Vector3f pos;
};

RaceCamera::RaceCamera(float aspectRatio) {
    mView.makeHomogeneous();
    // note that we're translating the scene in the reverse direction of where we want to move
    mView.setRotation(0, 0, 0);
    mView.setTranslation(0, 0, -3);

    mProjection.setPerspective(DEG2RAD(60), aspectRatio, 1, 5000);

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, sizeof(CameraData));
    //glCheckError();
}

void RaceCamera::onDraw() {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix44f), &mProjection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix44f), sizeof(Matrix44f), &mView);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix44f), sizeof(EGG::Quatf), &mRot);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix44f) +  sizeof(EGG::Quatf), sizeof(EGG::Vector3f), &mPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RaceCamera::setPos(const EGG::Vector3f& pos) {
    mView.setTranslation(pos.x, pos.y, pos.z);
    mPos = pos;
}

void RaceCamera::setRot(const EGG::Quatf& rot) {
    float qw = rot.w;
    float qx = rot.v.x;
    float qy = rot.v.y;
    float qz = rot.v.z;

    mView(0,0) = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
    mView(0,1) = 2.0f*qx*qy - 2.0f*qz*qw;
    mView(0,2) = 2.0f*qx*qz + 2.0f*qy*qw;

    mView(1,0) = 2.0f*qx*qy + 2.0f*qz*qw;
    mView(1,1) = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
    mView(1,2) = 2.0f*qy*qz - 2.0f*qx*qw;

    mView(2,0) = 2.0f*qx*qz - 2.0f*qy*qw;
    mView(2,1) = 2.0f*qy*qz + 2.0f*qx*qw;
    mView(2,2) = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
    mRot = rot;
}
