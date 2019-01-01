#ifndef PT_MATH_BOUNDS3_H
#define PT_MATH_BOUNDS3_H

#include <pt/core/ray.h>
#include <pt/math/vector3.h>

namespace pt {

class Bounds3 {
public:
    Bounds3() noexcept
        : pMin(std::numeric_limits<Float>::max())
        , pMax(std::numeric_limits<Float>::lowest())
    { }

    explicit Bounds3(const Vector3& p) noexcept : pMin(p), pMax(p)
    { }

    Bounds3(const Vector3& a, const Vector3& b) noexcept
        : pMin(min(a, b)), pMax(max(a, b))
    { }

    const Vector3& operator[](int index) const {
        return (&pMin)[index];
    }

    Vector3& operator[](int index) {
        return (&pMin)[index];
    }

    Bounds3& expandBy(const Vector3& p) {
        pMin = min(pMin, p);
        pMax = max(pMax, p);
        return *this;
    }

    Bounds3 expandBy(const Bounds3& b) {
        pMin = min(pMin, b.pMin);
        pMax = max(pMax, b.pMax);
        return *this;
    }

    Float area() const {
        auto d = diag();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    Vector3 diag() const {
        return pMax - pMin;
    }

    bool intersect(const Ray& ray, const Vector3& invDir, const int dirIsNeg[3]) const {
        auto& b = *this;
        auto tMin = (b[dirIsNeg[0]].x - ray.o.x) * invDir.x;
        auto tMax = (b[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
        if (tMax <= 0 || tMin >= ray.tMax) return false;
        tMin = std::max(tMin, (Float)0);
        tMax = std::min(tMax, ray.tMax);
        auto tyMin = (b[dirIsNeg[1]].y - ray.o.y) * invDir.y;
        auto tyMax = (b[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;
        if (tyMax <= tMin || tyMin >= tMax) return false;
        tMin = std::max(tMin, tyMin);
        tMax = std::min(tMax, tyMax);
        auto tzMin = (b[dirIsNeg[2]].z - ray.o.z) * invDir.z;
        auto tzMax = (b[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;
        if (tzMin >= tMax || tzMax <= tMin) return false;
        return true;
    }

    int maxExtent() const {
        auto d = diag();
        if (d.x > d.y && d.x > d.z) return 0;
        return d.y > d.z ? 1 : 2;
    }

    bool operator==(const Bounds3& b) const {
        return pMin == b.pMin && pMax == b.pMax;
    }

    bool operator!=(const Bounds3& b) const {
        return pMin != b.pMin || pMax != b.pMax;
    }

public:
    Vector3 pMin, pMax;
};

inline Bounds3 merge(const Bounds3& b, const Vector3& p) {
    return Bounds3(min(b.pMin, p), max(b.pMax, p));
}

inline Bounds3 merge(const Bounds3& a, const Bounds3& b) {
    return Bounds3(min(a.pMin, b.pMin), max(a.pMax, b.pMax));
}

}

#endif