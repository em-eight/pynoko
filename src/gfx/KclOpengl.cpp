#include "KclOpengl.hpp"

#include <vector>
#include <span>
#include "glad/glad.h"

#include <egg/math/Vector.hh>

static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1, const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm) {
    EGG::Vector3f cross = fnrm.cross(enrm);
    f32 dp = cross.ps_dot(enrm3);
    cross *= (height / dp);

    return cross + vertex1;
}

KclOpengl::KclOpengl(std::span<Field::KColData::KCollisionPrism> prisms, std::span<EGG::Vector3f> vertices, std::span<EGG::Vector3f> nrms) : m_prisms(prisms), m_vertices(vertices), m_nrms(nrms) {
    mShader = gfx::Shader("kcl.vert", "kcl.frag");
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
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),
    gfx::randomBrightColor(1.5f),

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
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
    gfx::randomBrightColor(1.0f),
};

void KclOpengl::processData() {
    for (const auto& prism : m_prisms) {
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
