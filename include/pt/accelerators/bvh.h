#ifndef PT_ACCELATORS_BVH_H
#define PT_ACCELATORS_BVH_H

#include <vector>
#include <pt/core/primitive.h>

namespace pt {

class BVHAccel : public Primitive {
public:
    BVHAccel(std::vector<Primitive*>&& primitives)
        : primitives(std::move(primitives))
    { }

    ~BVHAccel() {
        for (auto p : primitives) {
            delete p;
        }
    }

    bool intersect(const Ray& ray, Interaction& isect) const override {
        bool hit = false;
        for (auto p : primitives) {
            if (p->intersect(ray, isect)) hit = true;
        }
        return hit;
    }

    bool intersect(const Ray& ray) const override {
        for (auto p : primitives) {
            if (p->intersect(ray)) return true;
        }
        return false;
    }

private:
    std::vector<Primitive*> primitives;
};

}

#endif