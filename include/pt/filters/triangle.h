#ifndef PT_FILTERS_TRIANGLE_H
#define PT_FILTERS_TRIANGLE_H

#include <algorithm>
#include <pt/core/filter.h>

namespace pt {

class TriangleFilter : public Filter {
public:
    TriangleFilter(Float radius) : Filter(radius)
    { }

    Float evaluate(Float x) const override {
        return radius - std::abs(x);
    }
};

}

#endif

