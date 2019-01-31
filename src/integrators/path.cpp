#include <pt/core/bsdf.h>
#include <pt/core/light.h>
#include <pt/core/interaction.h>
#include <pt/core/visibilitytester.h>
#include <pt/integrators/path.h>

namespace pt {

Vector3 PathIntegrator::li(const Ray& ray, const Scene& scene) const {
    Ray r(ray);
    Vector3 l(0), beta(1);
    auto specularBounce = false;

    for (auto bounce = 0; bounce < maxDepth; ++bounce) {
        Interaction isect;
        auto foundIntersection = scene.intersect(r, isect);

        if (bounce == 0 || specularBounce) {
            if (foundIntersection) {
                l += beta * isect.le(-r.d);
            } else if (scene.infiniteLight) {
                l += beta * scene.infiniteLight->le(r);
            }
        }

        if (!foundIntersection) break;

        isect.computeScatteringFunctions();

        if (!isect.bsdf) break;

        l += beta * sampleOneLight(isect, scene);

        Float pdf;
        Vector3 wi, wo = -r.d;
        auto f = isect.bsdf->sampleF(sampler.get2D(), wo, wi, pdf);

        if (f == Vector3(0)) break;
        if (isect.bsdf->isDelta()) specularBounce = true;

        beta *= f * absdot(isect.n, wi) / pdf;
        r = isect.spawnRay(wi);

        // if (bounce > 3) {
        //     if (sampler.get1D() < (Float)0.8) break;
        //     beta *= 5;
        // }
    }

    return l;
}

Vector3 PathIntegrator::estimateDirect(
    const Interaction& isect,
    const Light& light,
    const Scene& scene) const {

    Vector3 wi;
    Float lightPdf;
    VisibilityTester tester;
    auto li = light.sampleLi(isect, sampler.get2D(), wi, lightPdf, tester);

    if (lightPdf == 0) return Vector3(0);

    Vector3 ld(0);

    if (li != Vector3(0)) {
        auto f = isect.bsdf->f(isect.wo, wi) * absdot(isect.n, wi);
        if (f != Vector3(0) && tester.unoccluded(scene)) {
            if (light.isDelta()) {
                ld += f * li / lightPdf;
            } else {
                auto scatteringPdf = isect.bsdf->pdf(isect.wo, wi);
                ld += f * li * powerHeuristic(lightPdf, scatteringPdf) / lightPdf;
            }
        }
    }

    if (!light.isDelta() && !isect.bsdf->isDelta()) {
        Vector3 wi;
        Float scatteringPdf;
        auto f = isect.bsdf->sampleF(sampler.get2D(), isect.wo, wi, scatteringPdf);
        f *= absdot(isect.n, wi);
        if (f != Vector3(0)) {
            lightPdf = light.pdf(isect, wi);
            if (lightPdf == 0) return ld;
            Interaction lightIsect;
            auto ray = isect.spawnRay(wi);
            auto foundIntersection = scene.intersect(ray, lightIsect);
            Vector3 li(0);
            if (foundIntersection) {
                if (lightIsect.primitive->getAreaLight() == &light)
                    li = lightIsect.le(-wi);
            } else {
                li = light.le(ray);
            }
            if (li != Vector3(0))
                ld += f * li * powerHeuristic(scatteringPdf, lightPdf) / scatteringPdf;
        }
    }

    return ld;
}

}
