#include <iostream>
#include <pt/core/distrib.h>
#include <pt/samplers/random.h>

using namespace pt;

int main() {
    std::vector<Float> pdf = { 0.1, 0.2, 0.3, 0.4 };
    auto distrib = Distribution1D(std::move(pdf));
    std::vector<int> counts = { 0, 0, 0, 0 };
    RandomSampler sampler(1);
    for (auto i = 0; i < 100000; ++i) {
        Float pdf;
        auto index = distrib.sampleDiscrete(sampler.get1D(), pdf);
        ++counts[index];
    }
    for (auto count : counts) {
        std::cout << count << " ";
    }
    std::cout << std::endl;
    return 0;
}