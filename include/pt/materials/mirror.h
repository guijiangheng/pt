#ifndef PT_MATERIALS_MIRROR_H
#define PT_MATERIALS_MIRROR_H

#include <pt/core/bsdf.h>
#include <pt/core/material.h>
#include <pt/bxdfs/specular.h>

namespace pt {

class MirrorMaterial : public Material {
public:
    explicit MirrorMaterial(const Vector3& r) noexcept : r(r)
    { }

    void computeScatteringFunctions(Interaction& isect) const override {
        if (r == Vector3(0)) return;
        isect.bsdf = new BSDF(isect);
        isect.bsdf->add(new SpecularReflection(r, new FresnelNoOp()));
    }

private:
    Vector3 r;
};

}

#endif
