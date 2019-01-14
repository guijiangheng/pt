#ifndef PT_MATERIALS_MATTE_H
#define PT_MATERIALS_MATTE_H

#include <pt/core/bsdf.h>
#include <pt/core/material.h>
#include <pt/bxdfs/lambert.h>

namespace pt {

class MatteMaterial : public Material {
public:
    explicit MatteMaterial(const Vector3& kd) noexcept : kd(kd)
    { }

    void computeScatteringFunctions(Interaction& isect) const override {
        if (kd == Vector3(0)) return;
        isect.bsdf = new BSDF(isect);
        isect.bsdf->add(new LambertReflection(kd));
    }

private:
    Vector3 kd;
};

}

#endif
