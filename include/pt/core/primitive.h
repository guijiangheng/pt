#ifndef PT_CORE_PRIMITIVE_H
#define PT_CORE_PRIMITIVE_H

#include <memory>
#include <pt/core/ray.h>
#include <pt/core/frame.h>
#include <pt/core/shape.h>
#include <pt/core/interaction.h>

namespace pt {

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;
    virtual bool intersect(const Ray& ray) const = 0;
};

class GeometricPrimitive : public Primitive {
public:
    GeometricPrimitive(const Frame& frame,
                       const std::shared_ptr<Shape>& shape)
        : frame(frame), shape(shape)
    { }

    bool intersect(const Ray& ray, Interaction& isect) const override {
        Float tHit;
        auto newRay = frame.toLocal(ray);
        if (!shape->intersect(newRay, tHit, isect)) return false;
        ray.tMax = tHit;
        isect = frame.toGlobal(isect);
        return true;
    }

    bool intersect(const Ray& ray) const override {
        return shape->intersect(frame.toLocal(ray));
    }

private:
    Frame frame;
    std::shared_ptr<Shape> shape;
};

}

#endif