#include "MkwVis.hpp"

#include <GLFW/glfw3.h>
#include "glad/glad.h"

#include <stdio.h>
#include <iostream>

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
    delete mKclOgl;
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

    // debug
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openGLDebugCallback, nullptr);

    glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void MkwVis::load() {
    mCamera = new gfx::Camera(mWidth, mHeight);
    mKclOgl = new KclOpengl(mKcl->m_prisms, mKcl->m_vertices, mKcl->m_nrms);
    mKclOgl->load();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera->onDraw();
    mKclOgl->draw();

    glfwSwapBuffers(mWindow);
}

void MkwVis::destroyWindow() {
    // TODO: destroy window
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
