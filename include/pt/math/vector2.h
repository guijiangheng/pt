#ifndef PT_MATH_VECTOR2_H
#define PT_MATH_VECTOR2_H

#include <pt/pt.h>

namespace pt {

template <typename T>
class Vector2 {
public:
    Vector2() = default;

    Vector2(T x, T y) : x(x), y(y)
    { }

    template <typename U>
    explicit Vector2(const Vector2<U>& rhs)
        : x((T)rhs.x)
        , y((T)rhs.y)
    { }

    Vector2 operator+(const Vector2& v) const {
        return Vector2(x + v.x, y + v.y);
    }

    Vector2 operator-(const Vector2& v) const {
        return Vector2(x - v.x, y - v.y);
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

}

#endif