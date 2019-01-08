#ifndef PT_LIGHTS_POINT_H
#define PT_LIGHTS_POINT_H

#include <pt/core/light.h>

namespace pt {

class PointLight : public Light {
public:
    PointLight(const Vector3& position, const Vector3& itensity)
        : Light(LightFlags::DeltaPosition)
        , position(position), itensity(itensity)
    { }

    Vector3 sampleLi(
        const Interaction& ref,
        const Vector2f& sample,
        Vector3& wi, Float& pdf, VisibilityTester& tester) const override {
        
        pdf = 1;
        wi = normalize(position - ref.p);
        tester = VisibilityTester(ref, position);

        // no need to divide 4 * pi, because point light use itensity not power
        return itensity / (position - ref.p).lengthSquared();
    }

public:
    Vector3 position;
    Vector3 itensity;
};

}

#endif
