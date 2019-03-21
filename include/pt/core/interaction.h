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
    explicit Interaction(const Vector3& p) noexcept
        : p(p), n(Vector3(0))
        , bsdf(nullptr), primitive(nullptr)
    { }

    Interaction(const Vector3& p, const Vector3& n, const Vector3& wo) noexcept
        : p(p), n(n), wo(wo)
        , bsdf(nullptr), primitive(nullptr)
    { }

    ~Interaction() noexcept;

    Vector3 offsetRayOrigin(const Vector3& w) const {
        return p + faceForward(n, w) * RayOriginOffsetEpsilon;
    }

    Ray spawnRay(const Vector3& wi) const {
        return Ray(offsetRayOrigin(wi), wi, Infinity);
    }

    Ray spawnRayTo(const Interaction& target) const {
        auto w = target.p - p;
        auto origin = offsetRayOrigin(normalize(w));
        return Ray(origin, w, 1 - ShadowEpsilon);
    }

    Vector3 le(const Vector3& wo) const;

    void computeScatteringFunctions();

public:
    Vector2f uv;
    Vector3 p;
    Vector3 n;
    Vector3 wo;
    BSDF* bsdf;
    const Primitive* primitive;
};

}

#endif
