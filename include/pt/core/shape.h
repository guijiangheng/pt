#ifndef PT_CORE_SHAPE_H
#define PT_CORE_SHAPE_H

#include <memory>
#include <pt/math/bounds3.h>
#include <pt/core/ray.h>
#include <pt/core/frame.h>
#include <pt/core/interaction.h>

namespace pt {

class Shape {
public:
    Shape(const Frame* frame) noexcept : frame(frame)
    { }

    virtual ~Shape() = default;

    virtual Bounds3 objectBound() const = 0;

    virtual Bounds3 worldBound() const {
        return frame->toWorld(objectBound());
    }

    virtual bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const = 0;

    virtual bool intersect(const Ray& ray) const {
        Float tHit;
        Interaction isect;
        return intersect(ray, tHit, isect);
    }

    virtual Float area() const = 0;

    virtual Interaction sample(const Vector2f& u, Float& pdf) const = 0;

    virtual Float pdf(const Interaction& p) const {
        return 1 / area();
    }

    virtual Interaction sample(const Interaction& ref, const Vector2f& u, Float& pdf) const {
        auto p = sample(u, pdf);
        auto d = p.p - ref.p;
        auto wi = normalize(d);
        pdf = d.lengthSquared() / absdot(p.n, wi);
        return p;
    }

    virtual Float pdf(const Interaction& ref, const Vector3& wi) const {
        auto ray = ref.spawnRay(wi);
        Float tHit;
        Interaction isect;
        if (!intersect(ray, tHit, isect)) return 0;
        return (isect.p - ref.p).lengthSquared() / (absdot(isect.n, wi) * area());
    }

public:
    const Frame* frame;
};

}

#endif
