#include "MkwVis.hpp"

using namespace bolt;
using namespace Kinoko;

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

    mScene->loadAll();
}

void MkwVis::setPose(const EGG::Vector3f& pos, const EGG::Quatf& rot) {
    mCamera->setPos(pos);
    mCamera->setRot(rot);
}

void MkwVis::draw() {
    mScene->draw(mCamera);
    mRenderSystem->readFramebuffer();
}

void MkwVis::destroy() {
    delete mCamera; mCamera = nullptr;
    delete mKclDrawable; mKclDrawable = nullptr;
    delete mScene; mScene = nullptr;
    delete mRenderSystem; mRenderSystem = nullptr;
}
