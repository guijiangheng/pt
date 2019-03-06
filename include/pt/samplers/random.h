#ifndef PT_SAMPLERS_RANDOM_H
#define PT_SAMPLERS_RANDOM_H

#include <random>
#include <pt/core/sampler.h>

namespace pt {

class RandomSampler : public Sampler {
public:
    explicit RandomSampler(std::uint64_t samplesPerPixel) noexcept
        : Sampler(samplesPerPixel)
        , device()
        , generator(device())
        , distribution(0, 1)
    { }

    std::unique_ptr<Sampler> clone(int seed) const override {
        return std::unique_ptr<Sampler>(new RandomSampler(samplesPerPixel));
    }

    Float get1D() override {
        return distribution(generator);
    }

    Vector2f get2D() override {
        return Vector2f(
            distribution(generator),
            distribution(generator)
        );
    }

private:
    std::random_device device;
    std::mt19937_64 generator;
    std::uniform_real_distribution<Float> distribution;
};

};

#endif
