#ifndef PT_CORE_RAY_H
#define PT_CORE_RAY_H

#include <pt/math/math.h>
#include <pt/math/vector3.h>

namespace pt {

class Ray {
public:
    Ray(const Vector3& o, const Vector3& d, Float tMax = Infinity) noexcept
        : o(o), d(d), tMax(tMax)
    { }

    Vector3 operator()(Float t) const {
        return o + d * t;
    }

public:
    Vector3 o;
    Vector3 d;
    mutable Float tMax;
};

}

#endif
