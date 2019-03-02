#ifndef PT_BXDFS_FRESNEL_H
#define PT_BXDFS_FRESNEL_H

#include <pt/core/bxdf.h>
#include <pt/core/fresnel.h>

namespace pt {

class FresnelSpecular : public BxDF {
public:
    FresnelSpecular(const Vector3& r, const Vector3& t, Float etaI, Float etaT) noexcept
        : BxDF(BxDFType(BXDF_SPECULAR | BXDF_REFLECTION | BXDF_TRANSMISSION))
        , r(r), t(t), etaI(etaI), etaT(etaT)
    { }

    Float pdf(const Vector3& wo, const Vector3& wi) const override {
        return 0;
    }

    Vector3 f(const Vector3& wo, const Vector3& wi) const override {
        return Vector3(0);
    }
    
    Vector3 sampleF(const Vector2f& u, const Vector3& wo, Vector3& wi, Float& pdf, Float& etaScale) const override {
        auto f = frDielectric(CoordinateSystem::cosTheta(wo), etaI, etaT);

        if (u[0] < f) {
            wi = Vector3(-wo.x, -wo.y, wo.z);
            pdf = f;
            etaScale = 1;
            return r * f / CoordinateSystem::absCosTheta(wi);
        }

        auto entering = CoordinateSystem::cosTheta(wo) > 0;
        auto tmpI = entering ? etaI : etaT;
        auto tmpT = entering ? etaT : etaI;
        refract(wo, faceForward(Vector3(0, 0, 1), wo), tmpI / tmpT, wi);
        pdf = 1 - f;
        etaScale = (tmpI * tmpI) / (tmpT * tmpT);

        return t * (1 - f) * etaScale / CoordinateSystem::absCosTheta(wi);
    }

private:
    Vector3 r, t;
    Float etaI, etaT;
};

}

#endif
