#ifndef PT_CORE_INTEGRATOR_H
#define PT_CORE_INTEGRATOR_H

#include <pt/core/scene.h>
#include <pt/core/camera.h>
#include <pt/core/sampler.h>

namespace pt {

class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void render(const Scene& scene) = 0;
};

class SamplerIntegrator : public Integrator {
public:
    SamplerIntegrator(Camera& camera, Sampler& sampler) noexcept
        : camera(camera), sampler(sampler)
    { }

    virtual Vector3 Li(const Ray& ray, const Scene& scene) const = 0;

    void render(const Scene& scene) override {
        for (auto p : camera.film.pixelBounds) {
            Vector3 color(0);
            sampler.startPixel();
            do {
                auto cameraSample = sampler.getCameraSample(p);
                auto ray = camera.generateRay(cameraSample);
                color += Li(ray, scene);
            } while (sampler.startNextSample());
            camera.film.pixels.emplace_back(color / sampler.samplesPerPixel);
        }
    }

private:
    Camera& camera;
    Sampler& sampler;
};

}

#endif