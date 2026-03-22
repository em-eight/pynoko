#pragma once

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include "game/field/KColData.hh"

#include "RaceCamera.hpp"
#include "KclDrawable.hpp"

#include "gfx/SceneManager.hpp"
#include "gfx/RenderSystem.hpp"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class MkwVis {
public:
    MkwVis(const Kinoko::Field::KColData* kcl) : mKcl(kcl) {}
    ~MkwVis();
    void createWindow(int width, int height);
    // call once to load graphics
    void load();
    // update character position
    void setPose(const Kinoko::EGG::Vector3f& pos, const Kinoko::EGG::Quatf& rot);
    // call every frame to process events (e.g. clicking 'X' button)
    void update();
    // call every frame to draw
    void draw();
    void destroyWindow();

private:
    bolt::gfx::SceneManager* mScene;
    bolt::gfx::RenderSystem* mRenderSystem;
    RaceCamera* mCamera;
    KclDrawable* mKclDrawable;
    const Kinoko::Field::KColData* mKcl;

    GLFWwindow* mWindow;
    int mWidth;
    int mHeight;
};
