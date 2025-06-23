#pragma once

#include "Drawable.hpp"

#include "math/Vector.hpp"

#include <string>
#include <vector>

namespace bolt {
namespace gfx {

struct MeshVertex {
    math::Vector3f position;
    math::Vector3f normal;
    math::Vector2f texCoo;
};

struct MeshTexture {
    unsigned int id;
    std::string type;
    std::string path;
};

class DrawableMesh : public Drawable3d {
public:
    DrawableMesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);

    virtual void draw() override;

private:
    void setupMesh();

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    // mesh Data
    std::vector<MeshVertex>   mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<MeshTexture>  mTextures;
};

} // gfx
} // bolt