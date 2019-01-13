#ifndef PT_CORE_COORDINATE_H
#define PT_CORE_COORDINATE_H

#include <pt/math/vector3.h>

namespace pt {

class CoordinateSystem {
public:
    explicit CoordinateSystem(const Vector3& n) noexcept : n(n) {
        coordinateSystem(n, s, t);
    }

    CoordinateSystem(const Vector3& n, const Vector3& s, const Vector3& t) noexcept
        : n(n), s(s), t(t)
    { }

    Vector3 toLocal(const Vector3& w) const {
        return Vector3(dot(w, s), dot(w, t), dot(w, n));
    }

    Vector3 toWorld(const Vector3& w) const {
        return s * w.x + t * w.y + n * w.z;
    }

    static Float cosTheta(const Vector3& w) {
        return w.z;
    }

    static Float absCosTheta(const Vector3& w) {
        return std::abs(w.z);
    }

    static bool sameHemisphere(const Vector3& wo, const Vector3& wi) {
        return wo.z * wi.z > 0;
    }

public:
    Vector3 n, s, t;
};

}

#endif
