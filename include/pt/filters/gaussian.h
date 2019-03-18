#ifndef PT_FILTERS_GAUSSIAN_H
#define PT_FILTERS_GAUSSIAN_H

#include <cmath>
#include <pt/core/filter.h>

namespace pt {

class GaussianFilter : public Filter {
public:
    GaussianFilter(Float radius, Float alpha)
        : Filter(radius), base(std::exp(-alpha * radius * radius))
    { }

    Float evaluate(Float x) const override {
        return std::exp(-alpha * x * x) - base;
    }

private:
    Float alpha, base;
};

}

#endif

