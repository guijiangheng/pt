#ifndef PT_CORE_INTERACTION_H
#define PT_CORE_INTERACTION_H

#include <pt/core/ray.h>

namespace pt {

class BSDF;
class Primitive;

class Interaction {
public:
    Interaction() noexcept : bsdf(nullptr), primitive(nullptr)
    { }

    // used in point light sampleLi method
    // initialize n to zero to make sure offsetRayOrigin doesn't offset the ray origin
    Interaction(const Vector3& p) noexcept
        : p(p), n(Vector3(0))
        , bsdf(nullptr), primitive(nullptr)
    { }

    Interaction(const Vector3& p, const Vector3& n, const Vector3& wo) noexcept
        : p(p), n(n), wo(wo)
        , bsdf(nullptr), primitive(nullptr)
    { }

    ~Interaction() noexcept;

    Vector3 offsetRayOrigin() const {
        return p + n * RayOriginOffsetEpsilon;
    }

    Ray spawnRay(const Vector3& wi) const {
        return Ray(offsetRayOrigin(), wi, Infinity);
    }

    Ray spawnRayTo(const Interaction& target) const {
        auto origin = offsetRayOrigin();
        auto dest = target.offsetRayOrigin();
        return Ray(origin, dest - origin, 1 - ShadowEpsilon);
    }

    Vector3 le(const Vector3& wo) const;

    void computeScatteringFunctions();

public:
    Vector3 p;
    Vector3 n;
    Vector3 wo;
    BSDF* bsdf;
    Primitive* primitive;
};

}

#endif
