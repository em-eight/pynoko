#include "KclOpengl.hpp"
#include "common.h"

#include <vector>
#include <span>
#include "glad/glad.h"

#include <egg/math/Vector.hh>

using namespace bolt;

static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1, const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm) {
    EGG::Vector3f cross = fnrm.cross(enrm);
    f32 dp = cross.ps_dot(enrm3);
    cross *= (height / dp);

    return cross + vertex1;
}

KclOpengl::KclOpengl(std::span<Field::KColData::KCollisionPrism> prisms, std::span<EGG::Vector3f> vertices, std::span<EGG::Vector3f> nrms) : m_prisms(prisms), m_vertices(vertices), m_nrms(nrms) {
    mShader = gfx::Shader(LIBMKW_RES("kcl.vert"), LIBMKW_RES("kcl.frag"));
    mShader.use();
    processData();
}

void KclOpengl::load() {
    // Generate and bind the VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Generate and bind the VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_triangleVertices.size() * sizeof(KclOglVtx), m_triangleVertices.data(), GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(KclOglVtx), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(KclOglVtx), (void*)(sizeof(EGG::Vector3f)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(KclOglVtx), (void*)(sizeof(EGG::Vector3f) + sizeof(EGG::Vector3f)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // set KCL pose (identity for course)
    mShader.use();
    math::Matrix44f mtx;
    mtx.setIdentity();
    mShader.setMat4("model", mtx);

    // Camera matrices (set by Camera)
    mShader.use();
    unsigned int uniformIndex = glGetUniformBlockIndex(mShader.id(), "Matrices");
    glUniformBlockBinding(mShader.id(), uniformIndex, 0);
}

void KclOpengl::draw() {
    mShader.use();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_triangleVertices.size());
    glBindVertexArray(0);
}

const gfx::Color colorTable[] = {
    gfx::Color(0.9f, 0.9f, 0.9f, 1.0f),      // 0x0 road
    gfx::Color(0.0f, 0.5f, 0.5f, 1.0f),      // 0x1 slippery road (no slow down)
    gfx::Color(1.0f, 0.85f, 0.3f, 1.0f),     // 0x2 weak off-road road
    gfx::Color(0.2f, 0.45f, 0.15f, 1.0f),    // 0x3 off-road road
    gfx::Color(0.5f, 0.25f, 0.0f, 1.0f),     // 0x4 heavy off-road road
    gfx::Color(0.05f, 0.6f, 0.95f, 1.0f),    // 0x5 slippery road road (slowdown)
    gfx::Color(1.0f, 0.3f, 0.0f, 1.0f),      // 0x6 boost
    gfx::Color(1.0f, 0.6f, 0.1f, 1.0f),      // 0x7 boost ramp
    gfx::Color(1.0f, 0.85f, 0.1f, 1.0f),     // 0x8 jump ramp
    gfx::Color(1.0f, 0.85f, 0.1f, 1.0f),     // 0x9 item road
    gfx::Color(0.25f, 0.1f, 0.0f, 1.0f),     // 0xA Solid fall
    gfx::Color(0.0f, 0.1f, 1.0f, 1.0f),      // 0xB Moving water
    gfx::Color(0.5f, 0.5f, 0.5f, 1.0f),      // 0xC Wall
    gfx::Color(0.75f, 0.75f, 0.75f, 1.0f),   // 0xD Invisible wall
    gfx::Color(0.85f, 0.85f, 0.75f, 1.0f),   // 0xE Item wall
    gfx::Color(0.5f, 0.5f, 0.5f, 1.0f),      // 0xF Wall 2

    gfx::Color(0.75f, 0.25f, 0.7f, 1.0f),    // 0x10 Fall boundary
    gfx::Color(0.65f, 0.4f, 0.5f, 1.0f),     // 0x11 Cannon trigger
    gfx::Color(0.65f, 0.4f, 0.5f, 1.0f),     // 0x12 Force recalculation
    gfx::Color(1.0f, 0.6f, 0.1f, 1.0f),      // 0x13 Half-pipe ramp
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
};

void KclOpengl::processData() {
    int i = 0;
    for (const auto& prism : m_prisms) {
        i++;
        if (i == 1) continue;
        const EGG::Vector3f &vtx1 = m_vertices[prism.pos_i];
        const EGG::Vector3f &fnrm = m_nrms[prism.fnrm_i];
        const EGG::Vector3f &enrm1 = m_nrms[prism.enrm1_i];
        const EGG::Vector3f &enrm2 = m_nrms[prism.enrm2_i];
        const EGG::Vector3f &enrm3 = m_nrms[prism.enrm3_i];

        EGG::Vector3f vtx2 = GetVertex(prism.height, vtx1, fnrm, enrm3, enrm1);
        EGG::Vector3f vtx3 = GetVertex(prism.height, vtx1, fnrm, enrm3, enrm2);

        u16 type = KCL_ATTRIBUTE_TYPE(prism.attribute);
        if (type == 0x12 || type == 0x1a || type == 0x18 || type == 0x1b) continue;
        gfx::Color color = colorTable[type];

        // Add the vertices to the triangle list
        m_triangleVertices.push_back(KclOglVtx(vtx1, fnrm, color));
        m_triangleVertices.push_back(KclOglVtx(vtx2, fnrm, color));
        m_triangleVertices.push_back(KclOglVtx(vtx3, fnrm, color));
    }
}
