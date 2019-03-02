#ifndef PT_CORE_BXDF_H
#define PT_CORE_BXDF_H

#include <pt/core/sampling.h>
#include <pt/core/coordinate.h>

namespace pt {

enum BxDFType {
    BXDF_REFLECTION         = 1 << 0,
    BXDF_TRANSMISSION       = 1 << 1,
    BXDF_DIFFUSE            = 1 << 2,
    BXDF_GLOSSY             = 1 << 3,
    BXDF_SPECULAR           = 1 << 4,
    BXDF_ALL                = BXDF_REFLECTION | BXDF_TRANSMISSION |
                              BXDF_DIFFUSE | BXDF_GLOSSY | BXDF_SPECULAR
};

class BxDF {
public:
    virtual ~BxDF() = default;

    explicit BxDF(BxDFType type) noexcept : type(type)
    { }

    bool match(BxDFType type) const {
        return (this->type & type) == type;
    }

    virtual Vector3 sampleF(const Vector2f& u, const Vector3& wo, Vector3& wi, Float& pdf, Float& etaScale) const {
        etaScale = 1;
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
