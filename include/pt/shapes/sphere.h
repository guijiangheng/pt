#ifndef PT_SHAPES_SPHERE_H
#define PT_SHAPES_SPHERE_H

#include <pt/core/shape.h>

namespace pt {

class Sphere : public Shape {
public:
    Sphere(Float radius = 1) : radius(radius)
    { }

    bool intersect(const Ray& r, Float& tHit, Interaction& isect) const override {
        auto a = r.d.lengthSquared();
        auto b = 2 * (r.d.x * r.o.x + r.d.y * r.o.y + r.d.z * r.o.z);
        auto c = r.o.lengthSquared() - radius * radius;
        Float t0, t1;
        if (!quadratic(a, b, c, t0, t1)) return false;
        if (t0 > r.tMax || t1 <= 0) return false;
        tHit = t0 <= 0 ? t1 : t0;
        isect.p = r(tHit);
        isect.n = normalize(isect.p);
        isect.wo = -r.d;
        return true;
    }

private:
    Float radius;
};

}

#endif