#ifndef PT_SHAPES_TRIANGLE_H
#define PT_SHAPES_TRIANGLE_H

#include <pt/core/shape.h>

namespace pt {

class Triangle : public Shape {
public:
    Triangle(const Vector3& a, const Vector3& b, const Vector3& c)
        : a(a), b(b), c(c)
    { }

    // ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
    bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const override {
        auto edge1 = b - a;
        auto edge2 = c - a;
        auto p = cross(ray.d, edge2);
        auto det = dot(p, edge1);

        if (det < Float(0.0001) && det > -Float(0.0001))
            return false;
        
        auto t = ray.o - a;
        auto detInv = 1 / det;
        auto u = dot(p, t) * detInv;
        if (u < 0 || u > 1) return false;

        auto q = cross(t, edge1);
        auto v = dot(q, ray.d) * detInv;
        if (v < 0 || u + v > 1) return false;

        auto dist = dot(q, edge2) * detInv;
        if (dist <= 0 || dist > ray.tMax) return false;

        tHit = dist;
        isect.p = a + edge1 * u + edge2 * v;
        isect.n = cross(edge2, edge1);
        isect.wo = -ray.d;

        return true;
    }

public:
    Vector3 a, b, c;
};

}

#endif