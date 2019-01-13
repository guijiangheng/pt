#ifndef PT_LIGHTS_POINT_H
#define PT_LIGHTS_POINT_H

#include <pt/core/light.h>
#include <pt/core/visibilitytester.h>

namespace pt {

class PointLight : public Light {
public:
    PointLight(const Vector3& position, const Vector3& intensity) noexcept
        : Light(LightFlags::DeltaPosition)
        , position(position), intensity(intensity)
    { }

    Vector3 sampleLi(
        const Interaction& ref,
        const Vector2f& sample,
        Vector3& wi, Float& pdf, VisibilityTester& tester) const override {
        
        pdf = 1;
        wi = normalize(position - ref.p);
        tester = VisibilityTester(ref, position);

        // no need to divide 4 * pi, because point light use intensity not power
        return intensity / (position - ref.p).lengthSquared();
    }

    Float pdf(const Interaction& ref, const Vector3& wi) const override {
        return 0;
    }

public:
    Vector3 position;
    Vector3 intensity;
};

}

#endif
