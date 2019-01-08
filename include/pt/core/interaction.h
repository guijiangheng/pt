#ifndef PT_CORE_INTERACTION_H
#define PT_CORE_INTERACTION_H

#include <pt/math/vector3.h>

namespace pt {

class Interaction {
public:
    Interaction() = default;

    Interaction(const Vector3& p, const Vector3& n, const Vector3& wo) noexcept
        : p(p), n(n), wo(wo)
    { }

    Vector3 offsetRayOrigin() const {
        return p + n * RayOriginOffsetEpsilon;
    }

    Ray spawnRay(const Vector3& wi) const {
        return Ray(offsetRayOrigin(), wi, Infinity);
    }

    Ray spawnRayTo(const Vector3& target) const {
        auto origin = offsetRayOrigin();
        return Ray(origin, target - origin, 1 - ShadowEpsilon);
    }

public:
    Vector3 p;
    Vector3 n;
    Vector3 wo;
};

}

#endif
