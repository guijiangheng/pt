#ifndef PT_MATH_VECTOR2_H
#define PT_MATH_VECTOR2_H

#include <cmath>
#include <pt/pt.h>

namespace pt {

template <typename T>
class Vector2 {
public:
    Vector2() = default;

    explicit Vector2(T x) noexcept : x(x), y(x)
    { }

    Vector2(T x, T y) noexcept : x(x), y(y)
    { }

    template <typename U>
    explicit Vector2(const Vector2<U>& v) noexcept : x((T)v.x), y((T)v.y)
    { }

    Float& operator[](int index) {
        return (&x)[index];
    }

    Float operator[](int index) const {
        return (&x)[index];
    }

    Vector2 operator+(const Vector2& v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x - v.x, y - v.y);
    }

    Vector2 operator*(T k) const {
        return Vector2(x * k, y * k);
    }

    Vector2& operator*=(T k) {
        x *= k;
        y *= k;
        return *this;
    }

    Vector2 operator/(T k) const {
        return Vector2(x / k, y / k);
    }

    Vector2& operator/=(T k) {
        auto inv = 1 / k;
        x *= inv;
        y *= inv;
        return *this;
    }

    bool operator==(const Vector2& v) const {
        return x == v.x && y == v.y;
    }

    bool operator!=(const Vector2& v) const {
        return x != v.x || y != v.y;
    }

public:
    T x, y;
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<Float>;

inline Vector2f floor(const Vector2f& p) {
    return Vector2f(std::floor(p.x), std::floor(p.y));
}

inline Vector2f ceil(const Vector2f& p) {
    return Vector2f(std::ceil(p.x), std::ceil(p.y));
}

template <typename T>
Vector2<T> abs(const Vector2<T>& p) {
    return Vector2<T>(std::abs(p.x), std::abs(p.y));
}

template <typename T>
Vector2<T> max(const Vector2<T>& a, const Vector2<T>& b) {
    return Vector2<T>(std::max(a.x, b.x), std::max(a.y, b.y));
}

template <typename T>
Vector2<T> min(const Vector2<T>& a, const Vector2<T>& b) {
    return Vector2<T>(std::min(a.x, b.x), std::min(a.y, b.y));
}

}

#endif
