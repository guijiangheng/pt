#ifndef PT_CORE_BSDF_H
#define PT_CORE_BSDF_H

#include <pt/core/bxdf.h>
#include <pt/core/interaction.h>

namespace pt {

class BSDF {
public:
    BSDF(const Interaction& isect) noexcept : nBxDFs(0), coord(isect.n)
    { }

    ~BSDF() noexcept {
        for (auto i = 0; i < nBxDFs; ++i)
            delete bxdfs[i];
    }

    void add(BxDF* bxdf) {
        bxdfs[nBxDFs++] = bxdf;
    }

    bool isDelta() const {
        if (nBxDFs == 1 && bxdfs[0]->match(BXDF_SPECULAR)) return true;
        return false;
    }

    Vector3 toLocal(const Vector3& w) const {
        return coord.toLocal(w);
    }

    Vector3 toWorld(const Vector3& w) const {
        return coord.toWorld(w);
    }

    Vector3 f(const Vector3& woWorld, const Vector3& wiWorld) const {
        Vector3 f(0);
        auto wo = toLocal(woWorld);
        auto wi = toLocal(wiWorld);
        auto reflect = dot(woWorld, coord.n) * dot(wiWorld, coord.n) > 0;
        for (auto i = 0; i < nBxDFs; ++i)
            if ((reflect && bxdfs[i]->match(BXDF_REFLECTION)) ||
                (!reflect && bxdfs[i]->match(BXDF_TRANSMISSION)))
                f += bxdfs[i]->f(wo, wi);
        return f;
    }

    Vector3 sampleF(
            const Vector2f& u,
            const Vector3& woWorld, Vector3& wiWorld,
            Float& pdf, Float& etaScale) const {

        int n = std::floor(u[0] * nBxDFs);
        Vector2f uRemapped(u[0] * nBxDFs - n, u[1]);
        Vector3 wi, wo = toLocal(woWorld);
        auto f = bxdfs[n]->sampleF(uRemapped, wo, wi, pdf, etaScale);

        for (auto i = 0; i < nBxDFs; ++i) {
            if (i == n) continue;
            pdf += bxdfs[i]->pdf(wo, wi);
        }

        pdf /= nBxDFs;
        wiWorld = toWorld(wi);
        auto reflect = dot(wo, coord.n) * dot(wi, coord.n) > 0;

        for (auto i = 0; i < n; ++i) {
            if (i == n) continue;
            if ((reflect && bxdfs[i]->match(BXDF_REFLECTION)) ||
                (!reflect && bxdfs[i]->match(BXDF_TRANSMISSION)))
                f += bxdfs[i]->f(wo, wi);
        }

        return f;
    }

    Float pdf(const Vector3& woWorld, const Vector3& wiWorld) const {
        auto wo = toLocal(woWorld);
        auto wi = toLocal(wiWorld);
        Float pdf = 0;
        for (auto i = 0; i < nBxDFs; ++i)
            pdf += bxdfs[i]->pdf(wo, wi);
        return pdf / nBxDFs;
    }

private:
    static constexpr int MaxBxDFs = 8;
    int nBxDFs;
    CoordinateSystem coord;
    BxDF* bxdfs[MaxBxDFs];
};

}

#endif
