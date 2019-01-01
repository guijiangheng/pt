#ifndef PT_CORE_PRIMITIVE_H
#define PT_CORE_PRIMITIVE_H

#include <pt/core/shape.h>

namespace pt {

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual Bounds3 getBounds() const = 0;
    virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;
    virtual bool intersect(const Ray& ray) const = 0;
};

class ShapePrimitive : public Primitive {
public:
    ShapePrimitive(const std::shared_ptr<Shape>& shape) noexcept : shape(shape)
    { }

    ShapePrimitive(const Frame& frame, const std::shared_ptr<Shape>& shape) noexcept
        : shape(std::make_shared<TransformedShape>(frame, shape))
    { }

    Bounds3 getBounds() const override {
        return shape->getBounds();
    }

    bool intersect(const Ray& ray, Interaction& isect) const override {
        Float tHit;
        if (!shape->intersect(ray, tHit, isect)) return false;
        ray.tMax = tHit;
        return true;
    }

    bool intersect(const Ray& ray) const override {
        return shape->intersect(ray);
    }

public:
    std::shared_ptr<Shape> shape;
};

}

#endif
