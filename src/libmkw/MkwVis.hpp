#pragma once

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include "game/field/KColData.hh"

#include "RaceCamera.hpp"
#include "KclDrawable.hpp"

#include "gfx/SceneManager.hpp"
#include "gfx/RenderSystem.hpp"
#include "gfx/opengl/OpenglFramebuffer.hpp"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class MkwVis {
public:
    MkwVis(const Kinoko::Field::KColData* kcl) : mKcl(kcl) {}
    ~MkwVis();
    // Creates a hidden window to host an OpenGL context, and an offscreen framebuffer of size
    // width x height that draw() renders into and getFrameBuffer() reads back from.
    void createWindow(int width, int height);
    // call once to load graphics
    void load();
    // update character position
    void setPose(const Kinoko::EGG::Vector3f& pos, const Kinoko::EGG::Quatf& rot);
    // call every frame to draw
    void draw();
    void destroyWindow();

    int width() const { return mWidth; }
    int height() const { return mHeight; }
    // size in bytes of the buffer returned by getFrameBuffer()
    int frameBufferSize() const { return mFramebuffer->bufferSize(); }
    // packed RGB floats of the last drawn frame, updated by draw()
    void* getFrameBuffer() const { return mFrameBufferData; }

private:
    bolt::gfx::SceneManager* mScene;
    bolt::gfx::RenderSystem* mRenderSystem;
    bolt::gfx::OpenglFramebuffer* mFramebuffer;
    RaceCamera* mCamera;
    KclDrawable* mKclDrawable;
    const Kinoko::Field::KColData* mKcl;

    GLFWwindow* mWindow;
    int mWidth;
    int mHeight;
    void* mFrameBufferData;
};
