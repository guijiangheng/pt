#ifndef PT_CORE_LIGHT_H
#define PT_CORE_LIGHT_H

#include <pt/core/scene.h>

namespace pt {

enum class LightFlags: int {
    DeltaPosition = 1,
    DeltaDirection = 2,
    Area = 4,
    Infinite = 8
};

class VisibilityTester {
public:
    VisibilityTester() = default;

    VisibilityTester(const Interaction& origin, const Interaction& target)
        : origin(origin), target(target)
    { }

    bool unoccluded(const Scene& scene) const {
        return !scene.intersect(origin.spawnRayTo(target));
    }

private:
    Interaction origin, target;
};

class Light {
public:
    virtual ~Light() = default;

    Light(LightFlags flags) : flags(flags)
    { }

    bool isDelta() const {
        return flags == LightFlags::DeltaPosition ||
               flags == LightFlags::DeltaDirection;
    }

    virtual Vector3 sampleLi(
        const Interaction& ref,
        const Vector2f& sample,
        Vector3& wi, Float& pdf, VisibilityTester& tester) const = 0;

public:
    LightFlags flags;    
};

}

#endif
