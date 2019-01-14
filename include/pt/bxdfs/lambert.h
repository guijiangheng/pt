#ifndef PT_BXDFS_LAMBERT_H
#define PT_BXDFS_LAMBERT_H

#include <pt/core/bxdf.h>

namespace pt {

class LambertReflection : public BxDF {
public:
    LambertReflection(const Vector3& kd) noexcept
        : BxDF(BxDFType(BXDF_REFLECTION | BXDF_DIFFUSE)), kd(kd)
    { }

    Vector3 f(const Vector3& wo, const Vector3& wi) const override {
        return kd * InvPi;
    }

private:
    Vector3 kd;
};

}

#endif
