#pragma once

#include "math/Matrix.hpp"

#include "gfx/Shader.hpp"

namespace bolt {
namespace gfx {

class Drawable {
public:
    virtual ~Drawable() = 0;
    /// Perform all heavy preparation necessary for draw() to work, e.g. load data from disk, send data to GPU. Must be called once defore the first draw() call
    virtual void load();
    /// Perform per-frame pre-draw preparations, e.g. send the model matrix to GPU if it's dirty
    virtual void onDraw();
    /// Perform a draw operation
    virtual void draw() = 0;
};

inline Drawable::~Drawable() {}
inline void Drawable::load() {};
inline void Drawable::onDraw() {};

class Drawable3d : public Drawable {
public:
    Drawable3d();

    virtual void onDraw() override;

    /// @beginGetters
    Shader& shader() { return mShader; }
    math::Matrix44f& mtx() { return mMtx; }
    /// @endGetters

    /// @beginSetters
    void setShader(const Shader& shader) { mShader = shader; }
    void setMtx(const math::Matrix34f& mtx);
    /// @endSetters

protected:
    Shader mShader;
    math::Matrix44f mMtx;
};

} // gfx
} // bolt