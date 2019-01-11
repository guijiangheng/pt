#include <pt/core/bsdf.h>
#include <pt/core/interaction.h>
#include <pt/integrators/path.h>

namespace pt {

Vector3 PathIntegrator::li(const Ray& ray, const Scene& scene) const {
    Ray r(ray);
    Vector3 l(0), beta(0);

    for (auto bounce = 0; bounce < maxDepth; ++bounce) {
        Interaction isect;
        auto foundIntersection = scene.intersect(r, isect);
        if (bounce == 0)
            l += foundIntersection ? isect.le(-r.d) : scene.infiniteLight.le(r);
        if (!foundIntersection) break;

        isect.computeScatteringFunctions();
        l += directLight(isect) * beta;

        Float pdf;
        Vector3 wi, wo = -r.d;
        auto f = isect.bsdf->sampleF(sampler.get2D(), wo, wi, pdf);
        if (f == Vector3(0)) break;
        beta *= f * absdot(isect.n, wi) / pdf;
        r = isect.spawnRay(wi);

        if (bounce > 3) {
            if (sampler.get1D() < (Float)0.8) break;
            beta *= 5;
        }
    }

    return l;
}

}
