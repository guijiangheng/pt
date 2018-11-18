#ifndef PT_MATH_MATH_H
#define PT_MATH_MATH_H

#include <cmath>
#include <limits>
#include <pt/pt.h>

namespace pt {

constexpr Float Infinity    = std::numeric_limits<Float>::infinity();
constexpr Float Pi          = (Float)3.14159265358979323846;
constexpr Float InvPi       = (Float)0.31830988618379067154;
constexpr Float Inv2Pi      = (Float)0.15915494309189533577;
constexpr Float Inv4Pi      = (Float)0.07957747154594766788;
constexpr Float PiOver2     = (Float)1.57079632679489661923;
constexpr Float PiOver4     = (Float)0.78539816339744830961;
constexpr Float Sqrt2       = (Float)1.41421356237309504880;

constexpr Float radians(Float deg) {
    return Pi / 180 * deg;
}

constexpr Float degrees(Float rad) {
    return 180 / Pi * rad;
}

inline bool quadratic(Float a, Float b, Float c, Float& t0, Float& t1) {
    double det = (double)b * (double)b - 4 * (double)a * (double)c;
    if (det < 0) return false;
    auto root = (Float)std::sqrt(det);
    auto q = b < 0 ? (root - b) / 2 : -(root + b) / 2;
    t0 = q / a;
    t1 = c / q;
    if (t0 > t1) std::swap(t0, t1);
    return true;
}

}

#endif