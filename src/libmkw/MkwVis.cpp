#include "MkwVis.hpp"

#include <GLFW/glfw3.h>

#include "gfx/opengl/OpenglRenderSystem.hpp"

#include <stdio.h>
#include <iostream>

using namespace bolt;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void GLAPIENTRY openGLDebugCallback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar *message, const void *userParam) {
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
    exit(-1);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

MkwVis::~MkwVis() {
    delete mCamera;
    delete mKclDrawable;
    delete mScene;
    delete mRenderSystem;
}

void MkwVis::createWindow(int width, int height) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init glfw\n");
    }

    glfwSetErrorCallback(error_callback);

    mWindow = glfwCreateWindow(width, height, "mkw", NULL, NULL);
    if (!mWindow) {
        fprintf(stderr, "Failed to create glfw window\n");
    }

    glfwMakeContextCurrent(mWindow);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetKeyCallback(mWindow, key_callback);
    glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);

    // bolt setup
    mRenderSystem = new bolt::gfx::OpenglRenderSystem;
    mScene = new bolt::gfx::SceneManager(mRenderSystem);
    mScene->renderSystem()->setViewport(0, 0, mWidth, mHeight);
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

void MkwVis::update() {
    glfwPollEvents();
    if (glfwWindowShouldClose(mWindow)) {
        destroyWindow();
    }
}

void MkwVis::draw() {
    mScene->draw(mCamera);

    glfwSwapBuffers(mWindow);
}

void MkwVis::destroyWindow() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
