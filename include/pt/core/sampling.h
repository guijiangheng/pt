#ifndef PT_CORE_SAMPLING_H
#define PT_CORE_SAMPLING_H

#include <pt/math/math.h>
#include <pt/math/vector3.h>

namespace pt {

inline Vector3 uniformSampleSphere(const Vector2f& u) {
    auto z = u[0];
    auto phi = 2 * Pi * u[1];
    auto r = std::max((Float)0, 1 - z * z);
    return Vector3(r * std::cos(phi), r * std::sin(phi), z);
}

inline Vector2f uniformSampleTriangle(const Vector2f& u) {
    auto u0 = std::sqrt(u[0]);
    return Vector2f(1 - u0, u[1] * u0);
}

inline Vector2f unifromSampleDisk(const Vector2f& u) {
    auto uSquare = u * (Float)2 - Vector2f(1, 1);
    if (uSquare.x == 0 && uSquare.y == 0) return Vector2f(0, 0);
    Float r, theta;
    if (std::abs(uSquare.x) > std::abs(uSquare.y)) {
        r = uSquare.x;
        theta = PiOver4 * (uSquare.y / uSquare.x);
    } else {
        r = uSquare.y;
        theta = PiOver2 - PiOver4 * (uSquare.x / uSquare.y);
    }
    return Vector2f(std::cos(theta), std::sin(theta)) * r;
}

inline Vector3 cosineSampleHemisphere(const Vector2f& u) {
    auto p = unifromSampleDisk(u);
    return Vector3(p.x, p.y, std::sqrt(std::max((Float)0, 1 - p.x * p.x - p.y * p.y)));
}

}

#endif
