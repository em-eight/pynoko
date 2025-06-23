#pragma once

#include "Drawable.hpp"
#include "Camera.hpp"

#include <vector>

namespace bolt {
namespace gfx {
/**
 * Simple linear container for graphics objects with a simple camera. Useful for prototyping PoC
 */
class Drawable3dManager {
public:
    Drawable3dManager();
    ~Drawable3dManager();
    void init();
    /// convenience function which calls Drawable3d::load() for all contained drawables
    void loadAll();
    /// draw all registered drawables
    void draw();
    void registerDrawable(Drawable3d* drawable);

    Camera* camera() { return mCamera; }

private:
    std::vector<Drawable3d*> mDrawables;
    Camera* mCamera;
};
} // gfx
} // bolt
