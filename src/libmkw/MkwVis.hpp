#pragma once

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include "game/field/KColData.hh"

#include "RaceCamera.hpp"
#include "KclOpengl.hpp"

#include <GLFW/glfw3.h>

class MkwVis {
public:
    MkwVis(Field::KColData* kcl) : mKcl(kcl) {}
    ~MkwVis();
    void createWindow(int width, int height);
    // call once to load graphics
    void load();
    // update character position
    void setPose(const EGG::Vector3f& pos, const EGG::Quatf& rot);
    // call every frame to process events (e.g. clicking 'X' button)
    void update();
    // call every frame to draw
    void draw();
    void destroyWindow();

private:
    RaceCamera* mCamera;
    KclOpengl* mKclOgl;
    Field::KColData* mKcl;

    GLFWwindow* mWindow;
    int mWidth;
    int mHeight;
};
