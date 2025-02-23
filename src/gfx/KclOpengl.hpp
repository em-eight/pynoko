#pragma once

#include <vector>
#include <span>

#include <egg/math/Vector.hh>
#include "game/field/KColData.hh"

#include "glad/glad.h"

#include "Shader.hpp"
#include "Color.hpp"

struct KclOglVtx {
    EGG::Vector3f vtx;
    EGG::Vector3f fnrm;
    gfx::Color color;

    KclOglVtx(EGG::Vector3f vtx, EGG::Vector3f fnrm, gfx::Color color) : vtx(vtx), fnrm(fnrm), color(color) {}
};

class KclOpengl {
public:
    KclOpengl(std::span<Field::KColData::KCollisionPrism> prisms, std::span<EGG::Vector3f> vertices, std::span<EGG::Vector3f> nrms);

    void load();
    void draw();

private:
    void processData();

    std::span<Field::KColData::KCollisionPrism> m_prisms;
    std::span<EGG::Vector3f> m_vertices;
    std::span<EGG::Vector3f> m_nrms;

    std::vector<KclOglVtx> m_triangleVertices;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    gfx::Shader mShader;
};
