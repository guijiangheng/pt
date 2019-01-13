#ifndef PT_CORE_BXDF_H
#define PT_CORE_BXDF_H

#include <pt/core/sampling.h>
#include <pt/core/coordinate.h>

namespace pt {

enum class BxDFType : int {
    Reflection      = 1 << 0,
    Transmission    = 1 << 1,
    Diffuse         = 1 << 2,
    Glossy          = 1 << 3,
    Specular        = 1 << 4,
    All             = Reflection | Transmission | Diffuse | Glossy | Specular
};

class BxDF {
public:
    virtual ~BxDF() = default;

    BxDF(BxDFType type) noexcept : type(type)
    { }

    bool match(const BxDFType& type) const {
        return ((int)this->type & (int)type) == (int)type;
    }

    virtual Vector3 sampleF(const Vector2f& u, const Vector3& wo, Vector3& wi, Float& pdf) const {
        wi = cosineSampleHemisphere(u);
        if (wo.z < 0) wi.z = -wi.z;
        pdf = CoordinateSystem::absCosTheta(wi) * InvPi;
        return f(wo, wi);
    }

    virtual Float pdf(const Vector3& wo, const Vector3& wi) const {
        return CoordinateSystem::sameHemisphere(wo, wi) ?
            CoordinateSystem::absCosTheta(wi) : 0;
    }

    virtual Vector3 f(const Vector3& wo, const Vector3& wi) const = 0;

public:
    BxDFType type;
};

}

#endif
