#ifndef PT_FILTERS_BOX_H
#define PT_FILTERS_BOX_H

#include <pt/core/filter.h>

namespace pt {

class BoxFilter : public Filter {
public:
    BoxFilter(Float radius) : Filter(radius)
    { }

    Float evaluate(Float x) const override {
        return 1;
    }
};

}

#endif

