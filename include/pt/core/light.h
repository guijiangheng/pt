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

    VisibilityTester(const Interaction& ref, const Vector3& target)
        : ref(ref), target(target)
    { }

    bool unoccluded(const Scene& scene) const {
        return !scene.intersect(ref.spawnRayTo(target));
    }

private:
    Interaction ref;
    Vector3 target;
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

    virtual Vector3 le(const Ray& ray) const {
        return Vector3(0);
    }

    virtual Vector3 sampleLi(
        const Interaction& ref,
        const Vector2f& sample,
        Vector3& wi, Float& pdf, VisibilityTester& tester) const = 0;
    
    virtual Float pdf(const Interaction& ref, const Vector3& wi) const = 0;

public:
    LightFlags flags;
};

class AreaLight : public Light {
public:
    AreaLight() : Light(LightFlags::Area)
    { }

    virtual Vector3 le(const Interaction& p, const Vector3& w) const = 0;
};

}

#endif
