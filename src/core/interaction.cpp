#include <pt/core/bsdf.h>
#include <pt/core/primitive.h>

namespace pt {

// no need to delete primitive, because it is managed by scene
Interaction::~Interaction() noexcept {
    if (bsdf) delete bsdf;
}

Vector3 Interaction::le(const Vector3& wo) const {
    auto areaLight = primitive->getAreaLight();
    return areaLight ? areaLight->le(*this, wo) : Vector3(0);
}

}
