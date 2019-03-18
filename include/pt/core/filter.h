#ifndef PT_CORE_FILTER_H
#define PT_CORE_FILTER_H

#include <pt/pt.h>
#include <pt/math/vector2.h>

namespace pt {

class Filter {
public:
    virtual ~Filter() = default;

    Filter(Float radius) : radius(radius), invRadius(1 / radius)
    { }

    virtual Float evaluate(Float x) const = 0;

public:
    Float radius, invRadius;
};

}

#endif

