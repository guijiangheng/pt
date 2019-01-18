#include <pt/core/fresnel.h>

namespace pt {

Float frDielectric(Float cosThetaI, Float etaI, Float etaT) {
    if (cosThetaI < 0) {
        cosThetaI = -cosThetaI;
        std::swap(etaI, etaT);
    }

    auto sinThetaI = std::sqrt(1 - cosThetaI * cosThetaI);
    auto sinThetaT = etaI / etaT * sinThetaI;

    if (sinThetaT >= 1) return 1;

    auto cosThetaT = std::sqrt(1 - sinThetaT * sinThetaT);
    auto rParl = (etaT * cosThetaI - etaI * cosThetaT) /
                 (etaT * cosThetaI + etaI * cosThetaT);
    auto rPerp = (etaI * cosThetaI - etaT * cosThetaT) /
                 (etaI * cosThetaI + etaT * cosThetaT);

    return (rParl * rParl + rPerp * rPerp) / 2;
}

}
