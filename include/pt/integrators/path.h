#ifndef PT_INTEGRATORS_PATH_H
#define PT_INTEGRATORS_PATH_H

#include <pt/core/integrator.h>

namespace pt {

class PathIntegrator : public SamplerIntegrator {
public:
    PathIntegrator(int maxDepth, Camera& camera, Sampler& sampler) noexcept
        : SamplerIntegrator(camera, sampler)
        , maxDepth(maxDepth)
    { }

    static Float powerHeuristic(Float a, Float b) {
        return (a * a) / (a * a + b * b);
    }

    Vector3 li(const Ray& ray, const Scene& scene) const override;

    Vector3 estimateDirect(
        const Interaction& isect,
        const Light& light,
        const Scene& scene) const;

    Vector3 sampleOneLight(const Interaction& isect, const Scene& scene) const {
        auto nLights = scene.lights.size();
        if (!nLights) return Vector3(0);
        auto light = scene.lights[(std::size_t)(sampler.get1D() * nLights)];
        return estimateDirect(isect, *light, scene) * nLights;
    }

public:
    int maxDepth;
};

}

#endif
