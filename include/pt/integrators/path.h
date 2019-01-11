#ifndef PT_INTEGRATORS_PATH_H
#define PT_INTEGRATORS_PATH_H

#include <pt/core/integrator.h>

namespace pt {

class PathIntegrator : public SamplerIntegrator {
public:
    PathIntegrator(int maxDepth, Camera& camera, Sampler& sampler)
        : SamplerIntegrator(camera, sampler)
        , maxDepth(maxDepth)
    { }

    Vector3 directLight(const Interaction& isect) const;

    Vector3 li(const Ray& ray, const Scene& scene) const override;

public:
    int maxDepth;
};

}

#endif
