#ifndef PT_MATH_VECTOR3_H
#define PT_MATH_VECTOR3_H

#include <cmath>
#include <pt/pt.h>

namespace pt {

class Vector3 {
public:
    Vector3() = default;

    Vector3(Float x, Float y, Float z) : e { x, y, z }
    { }

    Float& x() { return e[0]; }
    Float& y() { return e[1]; }
    Float& z() { return e[2]; }

    Float x() const { return e[0]; }
    Float y() const { return e[1]; }
    Float z() const { return e[2]; }

    Float& operator[](int index) {
        return e[index];
    }

    Float operator[](int index) const {
        return e[index];
    }

    Vector3 operator+(const Vector3& v) const {
        return Vector3(x() + v.x(), y() + v.y(), z() + v.z());
    }

    Vector3& operator+=(const Vector3& v) {
        x() += v.x();
        y() += v.y();
        z() += v.z();
        return *this;
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x() - v.x(), y() - v.y(), z() - v.z());
    }

    Vector3& operator-=(const Vector3& v) {
        x() -= v.x();
        y() -= v.y();
        z() -= v.z();
        return *this;
    }

    Vector3 operator*(Float k) const {
        return Vector3(x() * k, y() * k, z() * k);
    }

    Vector3& operator*=(Float k) {
        x() *= k;
        y() *= k;
        z() *= k;
        return *this;
    }

    Vector3 operator/(Float k) const {
        k = 1 / k;
        return Vector3(x() * k, y() * k, z() * k);
    }

    Vector3 operator/=(Float k) {
        k = 1 / k;
        x() *= k;
        y() *= k;
        z() *= k;
        return *this;
    }

    Vector3 operator-() const {
        return Vector3(-x(), -y(), -z());
    }

    Float lengthSquared() const {
        return x() * x() + y() * y() + z() * z();
    }

    Float length() const {
        return std::sqrt(lengthSquared());
    }

    Vector3& normalize() {
        *this /= length();
        return *this;        
    }

    bool operator==(const Vector3& v) const {
        return x() == v.x() && y() == v.y() && z() == v.z();
    }

    bool operator!=(const Vector3& v) const {
        return x() != v.x() || y() != v.y() || z() != v.z();
    }

private:
    Float e[3];
};

inline Float dot(const Vector3& a, const Vector3& b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

inline Float absdot(const Vector3& a, const Vector3& b) {
    return std::abs(dot(a, b));
}

inline Vector3 cross(const Vector3& a, const Vector3& b) {
    return Vector3(
        a.y() * b.z() - a.z() * b.y(),
        a.z() * b.x() - a.x() * b.z(),
        a.x() * b.y() - a.y() * b.x()
    );
}

inline Vector3 abs(const Vector3& v) {
    return Vector3(std::abs(v.x()), std::abs(v.y()), std::abs(v.z()));
}

inline Vector3 min(const Vector3& a, const Vector3& b) {
    return Vector3(
        std::min(a.x(), b.x()),
        std::min(a.y(), b.y()),
        std::min(a.z(), b.z())
    );
}

inline Vector3 max(const Vector3& a, const Vector3& b) {
    return Vector3(
        std::max(a.x(), b.x()),
        std::max(a.y(), b.y()),
        std::max(a.z(), b.z())
    );
}

inline Vector3 lerp(const Vector3& a, const Vector3& b, Float t) {
    return a + (b - a) * t;
}

inline Vector3 normalize(const Vector3& v) {
    return v / v.length();
}

inline Float distance(const Vector3& a, const Vector3& b) {
    return (a - b).length();
}

inline void coordinateSystem(const Vector3& a, Vector3& b, Vector3& c) {
    if (std::abs(a.x()) > std::abs(a.y()))
        b = Vector3(-a.z(), 0, a.x()) / std::sqrt(a.x() * a.x() + a.z() * a.z());
    else
        b = Vector3(0, -a.z(), a.y()) / std::sqrt(a.y() * a.y() + a.z() * a.z());
    c = cross(a, b);
}

}

#endif