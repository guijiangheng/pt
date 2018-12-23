#ifndef PT_CORE_SHAPE_H
#define PT_CORE_SHAPE_H

#include <memory>
#include <pt/core/ray.h>
#include <pt/core/interaction.h>

namespace pt {

class Shape {
public:
    virtual ~Shape() = default;

    virtual bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const = 0;

    virtual bool intersect(const Ray& ray) const {
        Float tHit;
        Interaction isect;
        return intersect(ray, tHit, isect);
    }
};

class TransformedShape : public Shape {
public:
    TransformedShape(const Frame& frame, const std::shared_ptr<Shape>& shape)
        : frame(frame), shape(shape)
    { }

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