#ifndef PT_LIGHTS_DIFFUSE_H
#define PT_LIGHTS_DIFFUSE_H

#include <pt/core/shape.h>
#include <pt/core/light.h>
#include <pt/core/visibilitytester.h>

namespace pt {

class DiffuseAreaLight : public Light {
public:
    DiffuseAreaLight(
        const std::shared_ptr<Shape>& shape,
        const Vector3& intensity,
        bool twoSided = false) noexcept
            : Light(LightFlags::Area)
            , shape(shape), intensity(intensity), twoSided(twoSided)
    { }

    DiffuseAreaLight(
        const Frame& frame,
        const std::shared_ptr<Shape>& shape,
        const Vector3& intensity,
        bool twoSided = false) noexcept
            : Light(LightFlags::Area)
            , shape(std::make_shared<TransformedShape>(frame, shape))
            , intensity(intensity), twoSided(twoSided)
    { }

    Vector3 le(const Interaction& pLight, const Vector3& wo) const {
        return (twoSided || dot(pLight.n, wo) > 0) ? intensity : Vector3(0);
    }

    Vector3 sampleLi(
        const Interaction& ref,
        const Vector2f& sample,
        Vector3& wi, Float& pdf, VisibilityTester& tester) const override {

        auto pLight = shape->sample(ref, sample, pdf);
        wi = normalize(pLight.p - ref.p);
        tester = VisibilityTester(ref, pLight);
        return le(pLight, -wi);
    }

    Float pdf(const Interaction& ref, const Vector3& wi) const override {
        return shape->pdf(ref, wi);
    }

public:
    std::shared_ptr<Shape> shape;
    Vector3 intensity;
    Float area;
    bool twoSided;
};

}

#endif
