#include "MkwVis.hpp"

#include "game/field/ObjectCollisionBox.hh"
#include "game/field/ObjectCollisionCylinder.hh"
#include "game/field/ObjectCollisionSphere.hh"
#include "game/field/ObjectDirector.hh"
#include "game/field/obj/ObjectCollidable.hh"
#include "game/field/obj/ObjectDrivable.hh"
#include "game/field/obj/ObjectKCL.hh"

#include "gfx/Color.hpp"
#include "gfx/DrawableCuboid.hpp"
#include "gfx/DrawableCylinder.hpp"
#include "gfx/DrawableSpheroid.hpp"

#include <cmath>

using namespace bolt;
using namespace Kinoko;

namespace {

math::Vector3f toBoltVec3(const EGG::Vector3f& v) {
    return math::Vector3f(v.x, v.y, v.z);
}

// calcTransform() never bakes scale into the rotation/translation matrix it produces, so it
// has to be folded into the local axes here.
math::Matrix34f toBoltMtx34(const EGG::Matrix34f& m, const EGG::Vector3f& scale) {
    math::Matrix34f out;
    for (int row = 0; row < 3; ++row) {
        out(row, 0) = m[row, 0] * scale.x;
        out(row, 1) = m[row, 1] * scale.y;
        out(row, 2) = m[row, 2] * scale.z;
        out(row, 3) = m[row, 3];
    }
    return out;
}

// DrawableCylinder is built along local +Z, so the cylinder's transform rotates +Z onto its
// current world-space axis. Returns false for a degenerate (zero-height) axis.
bool computeCylinderMtx(Field::ObjectCollisionCylinder* cylinder, math::Matrix34f& out) {
    EGG::Vector3f axisVec = cylinder->top() - cylinder->bottom();
    f32 height = axisVec.length();
    if (height < 1.0e-3f) {
        return false;
    }
    EGG::Vector3f axis = axisVec * (1.0f / height);

    EGG::Vector3f rotAxis = EGG::Vector3f::ez.cross(axis);
    f32 rotAxisLen = rotAxis.length();
    f32 cosAngle = EGG::Vector3f::ez.dot(axis);

    if (rotAxisLen < 1.0e-6f) {
        out.setRotation(math::Vector3f(1.0f, 0.0f, 0.0f), cosAngle > 0.0f ? 0.0f : static_cast<float>(M_PI));
    } else {
        out.setRotation(toBoltVec3(rotAxis * (1.0f / rotAxisLen)), std::acos(std::clamp(cosAngle, -1.0f, 1.0f)));
    }
    EGG::Vector3f center = cylinder->center();
    out.setTranslation(center.x, center.y, center.z);
    return true;
}

// ObjectCollisionBox always lays out 8 points where points[0] differs from points[1],
// points[2] and points[3] by exactly one flipped local axis each (z, y and x respectively), so
// those edges give the box's world-space orientation and extents directly, without assuming
// the box stays axis-aligned under rotation. Returns false for degenerate (zero-size) boxes.
bool computeBoxMtx(Field::ObjectCollisionBox* box, math::Matrix34f& out) {
    auto points = box->worldPoints();
    EGG::Vector3f right = points[0] - points[3];
    EGG::Vector3f up = points[0] - points[2];
    EGG::Vector3f fwd = points[0] - points[1];

    f32 width = right.length();
    f32 height = up.length();
    f32 depth = fwd.length();
    if (width < 1.0e-3f || height < 1.0e-3f || depth < 1.0e-3f) {
        return false;
    }
    right = right * (1.0f / width);
    up = up * (1.0f / height);
    fwd = fwd * (1.0f / depth);

    EGG::Vector3f center = EGG::Vector3f::zero;
    for (const EGG::Vector3f& p : points) {
        center += p;
    }
    center = center * (1.0f / static_cast<f32>(points.size()));

    out(0, 0) = right.x; out(1, 0) = right.y; out(2, 0) = right.z;
    out(0, 1) = up.x;    out(1, 1) = up.y;    out(2, 1) = up.z;
    out(0, 2) = fwd.x;   out(1, 2) = fwd.y;   out(2, 2) = fwd.z;
    out.setTranslation(center.x, center.y, center.z);
    return true;
}

} // namespace

MkwVis::~MkwVis() {
    destroy();
}

void MkwVis::create(int width, int height) {
    mWidth = width;
    mHeight = height;

    mRenderSystem = new bolt::gfx::VulkanRenderSystem(mWidth, mHeight);
    mRenderSystem->init();

    mScene = new bolt::gfx::SceneManager(mRenderSystem);
}

void MkwVis::load() {
    // camera
    mCamera = new RaceCamera;
    mCamera->setAspectRatio(mWidth / (float)mHeight);

    // course KCL
    mKclDrawable = new KclDrawable(mKcl->prisms(), mKcl->vertices(), mKcl->nrms());

    gfx::SceneNode& sceneRoot = mScene->root();
    sceneRoot.addChild(mCamera);
    sceneRoot.addChild(mKclDrawable);

    loadObjects();

    mScene->loadAll();
}

void MkwVis::loadObjects() {
    gfx::SceneNode& sceneRoot = mScene->root();

    for (Field::ObjectBase* obj : Field::ObjectDirector::Instance()->objects()) {
        if (auto* kclObj = dynamic_cast<Field::ObjectKCL*>(obj)) {
            const Field::KColData* kcl = kclObj->kclData();
            if (!kcl) {
                continue;
            }

            auto* drawable = new KclDrawable(kcl->prisms(), kcl->vertices(), kcl->nrms());
            sceneRoot.addChild(drawable);
            mObjectDrawables.push_back(drawable);
            mObjectUpdaters.push_back([kclObj, drawable]() {
                drawable->setMtx(toBoltMtx34(kclObj->getUpdatedMatrix(0), kclObj->scale()));
            });
            continue;
        }

        auto* collidable = dynamic_cast<Field::ObjectCollidable*>(obj);
        if (!collidable || !collidable->collision()) {
            continue;
        }

        collidable->calcCollisionTransform();
        Field::ObjectCollisionBase* collision = collidable->collision();

        if (auto* sphere = dynamic_cast<Field::ObjectCollisionSphere*>(collision)) {
            f32 radius = sphere->getBoundingRadius();
            auto* drawable = new gfx::DrawableSpheroid(radius, radius, radius);
            drawable->setColor(gfx::Color(1.0f, 0.0f, 1.0f, 1.0f));
            drawable->setTranslation(toBoltVec3(sphere->center()));
            sceneRoot.addChild(drawable);
            mObjectDrawables.push_back(drawable);
            mObjectUpdaters.push_back([collidable, sphere, drawable]() {
                collidable->calcCollisionTransform();
                drawable->setTranslation(toBoltVec3(sphere->center()));
            });
        } else if (auto* cylinder = dynamic_cast<Field::ObjectCollisionCylinder*>(collision)) {
            math::Matrix34f mtx;
            if (!computeCylinderMtx(cylinder, mtx)) {
                continue;
            }
            auto* drawable = new gfx::DrawableCylinder(
                    (cylinder->top() - cylinder->bottom()).length(), cylinder->getBoundingRadius());
            drawable->setColor(gfx::Color(0.0f, 1.0f, 1.0f, 1.0f));
            drawable->setMtx(mtx);
            sceneRoot.addChild(drawable);
            mObjectDrawables.push_back(drawable);
            mObjectUpdaters.push_back([collidable, cylinder, drawable]() {
                collidable->calcCollisionTransform();
                math::Matrix34f updatedMtx;
                if (computeCylinderMtx(cylinder, updatedMtx)) {
                    drawable->setMtx(updatedMtx);
                }
            });
        } else if (auto* box = dynamic_cast<Field::ObjectCollisionBox*>(collision)) {
            math::Matrix34f mtx;
            if (!computeBoxMtx(box, mtx)) {
                continue;
            }
            auto points = box->worldPoints();
            f32 width = (points[0] - points[3]).length();
            f32 height = (points[0] - points[2]).length();
            f32 depth = (points[0] - points[1]).length();
            auto* drawable = new gfx::DrawableCuboid(width, height, depth);
            drawable->setColor(gfx::Color(1.0f, 1.0f, 0.0f, 1.0f));
            drawable->setMtx(mtx);
            sceneRoot.addChild(drawable);
            mObjectDrawables.push_back(drawable);
            mObjectUpdaters.push_back([collidable, box, drawable]() {
                collidable->calcCollisionTransform();
                math::Matrix34f updatedMtx;
                if (computeBoxMtx(box, updatedMtx)) {
                    drawable->setMtx(updatedMtx);
                }
            });
        }
    }
}

void MkwVis::setPose(const EGG::Vector3f& pos, const EGG::Quatf& rot) {
    mCamera->setPos(pos);
    mCamera->setRot(rot);
}

void MkwVis::calc() {
    for (auto& update : mObjectUpdaters) {
        update();
    }
}

void MkwVis::draw() {
    mScene->draw(mCamera);
    mRenderSystem->readFramebuffer();
}

void MkwVis::destroy() {
    delete mCamera; mCamera = nullptr;
    delete mKclDrawable; mKclDrawable = nullptr;
    for (gfx::Drawable3d* drawable : mObjectDrawables) {
        delete drawable;
    }
    mObjectDrawables.clear();
    mObjectUpdaters.clear();
    delete mScene; mScene = nullptr;
    delete mRenderSystem; mRenderSystem = nullptr;
}
