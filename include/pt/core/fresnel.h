#ifndef PT_CORE_FRESNEL_H
#define PT_CORE_FRESNEL_H

#include <algorithm>
#include <pt/math/vector3.h>

namespace pt {

Float frDielectric(Float cosThetaI, Float etaI, Float etaT);

inline bool refract(const Vector3& wi, const Vector3& n, Float eta, Vector3& wt) {
    auto cosThetaI = dot(n, wi);
    auto sin2ThetaI = 1 - cosThetaI * cosThetaI;
    auto sin2ThetaT = eta * eta * sin2ThetaI;
    if (sin2ThetaT >= 1) return false;
    auto cosThetaT = std::sqrt(1 - sin2ThetaT);
    wt = -wi * eta + n * (eta * cosThetaI - cosThetaT);
    return true;
}

class Fresnel {
public:
    virtual ~Fresnel() = default;
    virtual Vector3 evaluate(Float cosThetaI) const = 0;
};

class FresnelNoOp : public Fresnel {
public:
    Vector3 evaluate(Float cosThetaI) const override {
        return Vector3(1);
    }
};

class FresnelDielectric : public Fresnel {
public:
    FresnelDielectric(Float etaI, Float etaT) noexcept : etaI(etaI), etaT(etaT)
    { }

    Vector3 evaluate(Float cosThetaI) const override {
        return Vector3(frDielectric(cosThetaI, etaI, etaT));
    }

private:
    Float etaI, etaT;
};

}

#endif
