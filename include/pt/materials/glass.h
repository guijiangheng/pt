#ifndef PT_MATERIALS_GLASS_H
#define PT_MATERIALS_GLASS_H

#include <pt/core/bsdf.h>
#include <pt/core/material.h>
#include <pt/bxdfs/fresnel.h>

namespace pt {

class GlassMaterial : public Material {
public:
    GlassMaterial(const Vector3& r, const Vector3& t, Float eta) noexcept
        : r(r), t(t), eta(eta)
    { }

    void computeScatteringFunctions(Interaction& isect) const override {
        isect.bsdf = new BSDF(isect);
        isect.bsdf->add(new FresnelSpecular(r, t, 1, eta));
    }

private:
    Vector3 r, t;
    Float eta;
};

}

#endif
