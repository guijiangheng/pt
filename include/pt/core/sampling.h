#ifndef PT_CORE_SAMPLING_H
#define PT_CORE_SAMPLING_H

#include <pt/math/math.h>
#include <pt/math/vector3.h>

namespace pt {

Vector3 uniformSampleSphere(const Vector2f& u) {
    auto z = u[0];
    auto phi = 2 * Pi * u[1];
    auto r = std::max((Float)0, 1 - z * z);
    return Vector3(r * std::cos(phi), r * std::sin(phi), z);
}

Vector2f uniformSampleTriangle(const Vector2f& u) {
    auto u0 = std::sqrt(u[0]);
    return Vector2f(1 - u0, u[1] * u0);
}

}

#endif
