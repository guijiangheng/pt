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
    virtual ~Shape() = default;

    virtual Bounds3 getBounds() const = 0;

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
};

class TransformedShape : public Shape {
public:
    TransformedShape(const Frame& frame, const std::shared_ptr<Shape>& shape) noexcept
        : frame(frame), shape(shape)
    { }

    Bounds3 getBounds() const override {
        return frame.toWorld(shape->getBounds());
    }

    bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const override {
        auto newRay = frame.toLocal(ray);
        if (!shape->intersect(newRay, tHit, isect)) return false;
        isect = frame.toWorld(isect);
        return true;
    }

    Float area() const override {
        return shape->area();
    }

    Interaction sample(const Vector2f& u, Float& pdf) const override {
        auto p = shape->sample(u, pdf);
        Interaction isect;
        isect.p = frame.toWorldP(p.p);
        isect.n = frame.toWorldN(p.n);
        return isect;
    }

public:
    Frame frame;
    std::shared_ptr<Shape> shape;
};

}

#endif
