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

public:
    Frame frame;
    std::shared_ptr<Shape> shape;
};

}

#endif
