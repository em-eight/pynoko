#pragma once

#include "Drawable.hpp"
#include "DrawableMesh.hpp"

#include <assimp/scene.h>

#include <vector>
#include <string>

namespace bolt {
namespace gfx {

class DrawableModel : public Drawable3d {
public:
    DrawableModel(const char* path);
    ~DrawableModel();
    
    virtual void load() override;
    virtual void draw() override;

private:
    void loadModel(const char* path);
    void processNode(aiNode *node, const aiScene *scene);
    DrawableMesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<MeshTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

    std::vector<DrawableMesh> mMeshes;
    std::string mDirectory;
    std::vector<MeshTexture> textures_loaded;
    char* mPath;
};

} // gfx
} // bolt