#ifndef PT_SHAPES_SPHERE_H
#define PT_SHAPES_SPHERE_H

#include <pt/core/shape.h>
#include <pt/core/sampling.h>

namespace pt {

class Sphere : public Shape {
public:
    Sphere(const Frame* frame, Float radius = 1) noexcept : Shape(frame), radius(radius)
    { }

    Bounds3 objectBound() const override {
        return Bounds3(Vector3(-radius), Vector3(radius));
    }

    bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const override {
        auto r = frame->toLocal(ray);
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
        isect = frame->toWorld(isect);
        return true;
    }

    Float area() const override {
        return 4 * Pi * radius * radius;
    }

    Interaction sample(const Vector2f& u, Float& pdf) const override {
        Interaction isect;
        auto n = uniformSampleSphere(u);
        isect.n = frame->toWorldN(n);
        isect.p = frame->toWorldP(n * radius);
        isect.p = isect.n * radius;
        pdf = 1 / area();
        return isect;
    }

private:
    Float radius;
};

}

#endif
