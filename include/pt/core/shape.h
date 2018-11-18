#ifndef PT_CORE_SHAPE_H
#define PT_CORE_SHAPE_H

#include "ray.h"
#include "interaction.h"

namespace pt {

class Shape {
public:
    virtual bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const = 0;

    virtual bool intersect(const Ray& ray) const {
        Float tHit;
        Interaction isect;
        return intersect(ray, tHit, isect);
    }
};

}

#endif