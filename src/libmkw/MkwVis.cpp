#include "MkwVis.hpp"

#include <GLFW/glfw3.h>

#include "gfx/opengl/OpenglRenderSystem.hpp"

#include <cstdlib>
#include <stdio.h>

using namespace bolt;
using namespace Kinoko;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

MkwVis::~MkwVis() {
    delete mCamera;
    delete mKclDrawable;
    delete mScene;
    delete mRenderSystem;
    delete mFramebuffer;
    free(mFrameBufferData);
}

void MkwVis::createWindow(int width, int height) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init glfw\n");
    }

    glfwSetErrorCallback(error_callback);

    // hidden window, used only to host an OpenGL context for offscreen rendering
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(width, height, "mkw", NULL, NULL);
    if (!mWindow) {
        fprintf(stderr, "Failed to create glfw window\n");
    }

    glfwMakeContextCurrent(mWindow);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);

    // bolt setup
    mRenderSystem = new bolt::gfx::OpenglRenderSystem;
    mScene = new bolt::gfx::SceneManager(mRenderSystem);
    mScene->renderSystem()->setViewport(0, 0, mWidth, mHeight);

    mFramebuffer = new bolt::gfx::OpenglFramebuffer(mWidth, mHeight);
    mFramebuffer->use();
    mFrameBufferData = malloc(mFramebuffer->bufferSize());
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
    mFramebuffer->readBuffer(mFrameBufferData);
}

void MkwVis::destroyWindow() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
