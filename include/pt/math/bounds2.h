#ifndef PT_MATH_BOUNDS2_H
#define PT_MATH_BOUNDS2_H

#include <limits>
#include <iterator>
#include <pt/math/vector2.h>

namespace pt {

template <typename T>
class Bounds2 {
public:
    Bounds2() {
        auto minVal = std::numeric_limits<T>::lowest();
        auto maxVal = std::numeric_limits<T>::max();
        pMin = Vector2(maxVal, maxVal);
        pMax = Vector2(minVal, minVal);
    }

    explicit Bounds2(const Vector2<T>& p) : pMin(p), pMax(p)
    { }

    Bounds2(const Vector2<T>& pMin, const Vector2<T>& pMax) : pMin(pMin), pMax(pMax)
    { }

    Vector2<T> diag() const {
        return pMax - pMin;
    }

    T area() const {
        auto d = diag();
        return d.x * d.y;
    }

public:
    Vector2<T> pMin, pMax;
};

using Bounds2i = Bounds2<int>;
using Bounds2f = Bounds2<Float>;

class Bounds2iIterator : public std::forward_iterator_tag {
public:
    Bounds2iIterator(const Bounds2i& bounds, const Vector2i& p)
        : bounds(&bounds), p(p)
    { }

    Bounds2iIterator& operator++() {
        advance();
        return *this;
    }

    Bounds2iIterator operator++(int) {
        auto old = *this;
        advance();
        return old;
    }

    Vector2i operator*() const {
        return p;
    }

    bool operator==(const Bounds2iIterator& rhs) const {
        return p == rhs.p && bounds == rhs.bounds;
    }

    bool operator!=(const Bounds2iIterator& rhs) const {
        return p != rhs.p || bounds != rhs.bounds;
    }

private:
    void advance() {
        ++p.x;
        if (p.x == bounds->pMax.x) {
            p.x = bounds->pMin.x;
            ++p.y;
        }
    }

private:
    Vector2i p;
    const Bounds2i* bounds;
};

inline Bounds2iIterator begin(const Bounds2i& bounds) {
    return Bounds2iIterator(bounds, bounds.pMin);
}

inline Bounds2iIterator end(const Bounds2i& bounds) {
    return Bounds2iIterator(bounds, Vector2i(bounds.pMin.x, bounds.pMax.y));
}

}

#endif