#pragma once

#include <egg/math/Vector.hh>
#include <egg/math/Quat.hh>
#include "game/field/KColData.hh"

#include "RaceCamera.hpp"
#include "KclDrawable.hpp"

#include "gfx/SceneManager.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

class MkwVis {
public:
    MkwVis(const Kinoko::Field::KColData* kcl) : mKcl(kcl) {}
    ~MkwVis();
    // Creates a headless Vulkan render system with an offscreen target of size width x height
    // that draw() renders into and getFrameBuffer() reads back from.
    void create(int width, int height);
    // call once to load graphics
    void load();
    // update character position
    void setPose(const Kinoko::EGG::Vector3f& pos, const Kinoko::EGG::Quatf& rot);
    // call every frame to draw
    void draw();
    void destroy();

    int width() const { return mWidth; }
    int height() const { return mHeight; }
    // size in bytes of the buffer returned by getFrameBuffer()
    int frameBufferSize() const { return mWidth * mHeight * 4; }
    // packed RGBA8 pixels of the last drawn frame, updated by draw(); owned by the render system,
    // valid until the next draw() call
    void* getFrameBuffer() const { return mRenderSystem->getFrameBuffer(); }

private:
    bolt::gfx::SceneManager* mScene;
    bolt::gfx::VulkanRenderSystem* mRenderSystem;
    RaceCamera* mCamera;
    KclDrawable* mKclDrawable;
    const Kinoko::Field::KColData* mKcl;

    int mWidth;
    int mHeight;
};
