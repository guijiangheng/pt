#ifndef PT_BXDFS_SPECULAR_H
#define PT_BXDFS_SPECULAR_H

#include <pt/core/bxdf.h>
#include <pt/core/fresnel.h>
#include <pt/core/coordinate.h>

namespace pt {

class SpecularReflection : public BxDF {
public:
    SpecularReflection(const Vector3& r, Fresnel* fresnel) noexcept
        : BxDF(BxDFType(BXDF_SPECULAR | BXDF_REFLECTION))
        , r(r), fresnel(fresnel)
    { }

    ~SpecularReflection() {
        delete fresnel;
    }

    Float pdf(const Vector3& wo, const Vector3& wi) const override {
        return 0;
    }

    Vector3 f(const Vector3& wo, const Vector3& wi) const override {
        return Vector3(0);
    }

    Vector3 sampleF(const Vector2f& u, const Vector3& wo, Vector3& wi, Float& pdf) const override {
        wi = Vector3(-wo.x, -wo.y, wo.z);
        pdf = 1;
        return r * fresnel->evaluate(CoordinateSystem::cosTheta(wi)) / CoordinateSystem::absCosTheta(wi);
    }

private:
    Vector3 r;
    Fresnel* fresnel;
};

}

#endif
