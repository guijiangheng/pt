#ifndef PT_CORE_PRIMITIVE_H
#define PT_CORE_PRIMITIVE_H

#include <memory>
#include <pt/core/frame.h>

#include "ray.h"
#include "shape.h"
#include "interaction.h"

namespace pt {

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual bool intersect(const Ray& r, Interaction& isect) const = 0;
    virtual bool intersect(const Ray& r) const = 0;
};

class GeometricPrimitive : public Primitive {
public:
    GeometricPrimitive(const Frame& frame,
                       const std::shared_ptr<Shape>& shape)
        : frame(frame), shape(shape)
    { }

    bool intersect(const Ray& r, Interaction& isect) const override {
        Float tHit;
        auto newRay = frame.toLocal(r);
        if (!shape->intersect(newRay, tHit, isect)) return false;
        r.tMax = tHit;
        isect = frame.toGlobal(isect);
        return true;
    }

    bool intersect(const Ray& r) const override {
        return shape->intersect(frame.toLocal(r));
    }

private:
    Frame frame;
    std::shared_ptr<Shape> shape;
};

}

#endif