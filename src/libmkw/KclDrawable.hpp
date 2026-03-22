#pragma once

#include <vector>
#include <span>

#include <egg/math/Vector.hh>
#include "game/field/KColData.hh"

#include "gfx/Color.hpp"
#include "gfx/Drawable.hpp"

struct KclVtx {
    Kinoko::EGG::Vector3f vtx;
    Kinoko::EGG::Vector3f fnrm;
    bolt::gfx::Color color;

    KclVtx(Kinoko::EGG::Vector3f vtx, Kinoko::EGG::Vector3f fnrm, bolt::gfx::Color color) : vtx(vtx), fnrm(fnrm), color(color) {}
};

class KclDrawable : public bolt::gfx::Drawable3d {
public:
    KclDrawable(std::span<const Kinoko::Field::KColData::KCollisionPrism> prisms, std::span<const Kinoko::EGG::Vector3f> vertices, std::span<const Kinoko::EGG::Vector3f> nrms);

    virtual const bolt::gfx::VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const bolt::gfx::ProgramDescriptor& programDescriptor() const override;
    virtual const void* vertexData() const override { return m_triangleVertices.data(); }
    virtual uint64_t vertexCount() const override { return m_triangleVertices.size(); }
    virtual bolt::gfx::DrawOp drawOp() const override { return bolt::gfx::BOLT_GFX_ARRAY; }

private:
    void processData();

    std::span<const Kinoko::Field::KColData::KCollisionPrism> m_prisms;
    std::span<const Kinoko::EGG::Vector3f> m_vertices;
    std::span<const Kinoko::EGG::Vector3f> m_nrms;

    std::vector<KclVtx> m_triangleVertices;
};
