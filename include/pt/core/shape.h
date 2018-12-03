#ifndef PT_CORE_SHAPE_H
#define PT_CORE_SHAPE_H

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

}

#endif