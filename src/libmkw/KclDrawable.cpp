#include "KclDrawable.hpp"
#include "common.h"

#include <vector>
#include <span>

#include <egg/math/Vector.hh>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

using namespace bolt;
using namespace bolt::gfx;

static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1, const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm) {
    EGG::Vector3f cross = fnrm.cross(enrm);
    f32 dp = cross.ps_dot(enrm3);
    cross *= (height / dp);

    return cross + vertex1;
}

static Color randomBrightColor(float minBrightness) {
    RUNTIME_ASSERT(minBrightness < 2.0f, "minBrightness must be between 0 and 2");

    Color ret;
    ret.a = 1.0f;
    do {
        ret.r = static_cast<float>(rand()) / RAND_MAX;
        ret.g = static_cast<float>(rand()) / RAND_MAX;
        ret.b = static_cast<float>(rand()) / RAND_MAX;
    } while (ret.r + ret.g + ret.b < minBrightness);

    return ret;
}

KclDrawable::KclDrawable(std::span<Field::KColData::KCollisionPrism> prisms, std::span<EGG::Vector3f> vertices, std::span<EGG::Vector3f> nrms) : m_prisms(prisms), m_vertices(vertices), m_nrms(nrms) {
    processData();
}

// Vertex attribute layout:
// Attribute 0: position (3 floats) -> offset 0
// Attribute 1: normal (3 floats) -> offset 12
// Attribute 2: color (4 floats) -> offset 20
static const VertexAttribute KCL_VTX_ATTR[] = {
    {0, offsetof(KclVtx, vtx), 3, BOLT_F32, sizeof(KclVtx)}, // position
    {1, offsetof(KclVtx, fnrm), 3, BOLT_F32, sizeof(KclVtx)},   // normal
    {2, offsetof(KclVtx, color), 4, BOLT_F32, sizeof(KclVtx)}    // texture coordinates
};

static const ProgramDescriptor KCL_PROG_DESC = {
    LIBMKW_RES("kcl.vert"),
    LIBMKW_RES("kcl.frag")
};

const VertexAttribute* KclDrawable::attributes() const {
    return KCL_VTX_ATTR;
}

int KclDrawable::attributeCount() const {
    return ARRAY_SIZE(KCL_VTX_ATTR);
}

const ProgramDescriptor& KclDrawable::programDescriptor() const {
    return KCL_PROG_DESC;
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
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
    randomBrightColor(1.0f),
};

void KclDrawable::processData() {
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
        m_triangleVertices.push_back(KclVtx(vtx1, fnrm, color));
        m_triangleVertices.push_back(KclVtx(vtx2, fnrm, color));
        m_triangleVertices.push_back(KclVtx(vtx3, fnrm, color));
    }
}
