#ifndef PT_CORE_SAMPLER_H
#define PT_CORE_SAMPLER_H

#include <cstdint>
#include <pt/math/vector2.h>
#include <pt/core/camera.h>

namespace pt {

class Sampler {
public:
    virtual ~Sampler() = default;

    explicit Sampler(std::int64_t samplesPerPixel) noexcept
        : samplesPerPixel(samplesPerPixel)
    { }

    virtual Float get1D() = 0;
    
    virtual Vector2f get2D() = 0;

    void startPixel() {
        currentPixelSampleIndex = 0;
    }

    bool startNextSample() {
        return ++currentPixelSampleIndex < samplesPerPixel;
    }

    CameraSample getCameraSample(const Vector2i& pRaster) {
        return CameraSample {
            (Vector2f)pRaster + get2D(),
            get2D()
        };
    }

public:
    std::int64_t samplesPerPixel;

protected:
    std::int64_t currentPixelSampleIndex;
};

}

#endif