#ifndef PT_CORE_BSDF_H
#define PT_CORE_BSDF_H

#include <pt/core/bxdf.h>
#include <pt/core/interaction.h>

namespace pt {

class BSDF {
public:
    BSDF(const Interaction& isect) : nBxDFs(0), coord(isect.n)
    { }

    ~BSDF() {
        for (auto i = 0; i < nBxDFs; ++i)
            delete bxdfs[i];
    }

    void add(BxDF* bxdf) {
        bxdfs[nBxDFs++] = bxdf;
    }

    Vector3 toLocal(const Vector3& w) const {
        return coord.toLocal(w);
    }

    Vector3 toWorld(const Vector3& w) const {
        return coord.toWorld(w);
    }

    Vector3 f(const Vector3& woWorld, const Vector3& wiWorld) const {
        auto wo = toLocal(woWorld);
        auto wi = toLocal(wiWorld);
        auto reflect = dot(wo, coord.n) * dot(wi, coord.n) > 0;
        Vector3 f(0);
        for (auto i = 0; i < nBxDFs; ++i)
            if (( reflect && bxdfs[i]->match(BxDFType::Reflection)) ||
                (!reflect && bxdfs[i]->match(BxDFType::Transmission)))
                f += bxdfs[i]->f(wo, wi);
    }

    Vector3 sampleF(const Vector3& woWorld, Vector3& wiWorld, const Vector2f& u, Float& pdf) const {
        int n = std::floor(u[0] * nBxDFs);
    }


private:
    static constexpr int MaxBxDFs = 8;
    CoordinateSystem coord;
    int nBxDFs;
    BxDF* bxdfs[MaxBxDFs];
};

}

#endif
