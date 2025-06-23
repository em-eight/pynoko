#include "gfx/DrawableManager.hpp"
#include "gfx/gl_defines.h"

#include "util/common.h"

namespace bolt {
namespace gfx {

Drawable3dManager::Drawable3dManager() {

}

void Drawable3dManager::init() {
    mCamera = new OrbitCamera;
}

void Drawable3dManager::loadAll() {
    for (const auto& drawable : mDrawables) {
        RUNTIME_ASSERT(drawable != nullptr, "null drawable");
        drawable->load();
    }
}

Drawable3dManager::~Drawable3dManager() {
    delete mCamera;
}

void Drawable3dManager::draw() {
    mCamera->onDraw();
    for (const auto& drawable : mDrawables) {
        RUNTIME_ASSERT(drawable != nullptr, "null drawable");
        drawable->onDraw();
        drawable->shader().use();
        drawable->draw();
    }
}

void Drawable3dManager::registerDrawable(Drawable3d* drawable) {
    // Bind uniform of camera projection and view matrices to 0
    Shader& shader = drawable->shader();
    shader.use();
    unsigned int uniformIndex = glGetUniformBlockIndex(shader.id(), "Matrices");
    glUniformBlockBinding(shader.id(), uniformIndex, 0);
    glCheckError();

    mDrawables.push_back(drawable);
}

} // gfx
} // bolt
