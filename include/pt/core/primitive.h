#ifndef PT_CORE_PRIMITIVE_H
#define PT_CORE_PRIMITIVE_H

#include <pt/core/shape.h>
#include <pt/core/material.h>
#include <pt/lights/diffuse.h>

namespace pt {

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual Bounds3 getBounds() const = 0;
    virtual bool intersect(const Ray& ray, Interaction& isect) const = 0;
    virtual bool intersect(const Ray& ray) const = 0;
    virtual const Material* getMaterial() const = 0;
    virtual const DiffuseAreaLight* getAreaLight() const = 0;
    virtual void computeScatteringFunctions(Interaction& isect) const = 0;
};

class ShapePrimitive : public Primitive {
public:
    // careful, primitive should only set neither material or light property, but not both
    // if area light is setted, area light's shape and primitive's shape
    // should point to same shape
    ShapePrimitive(
        const std::shared_ptr<Shape>& shape,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<DiffuseAreaLight>& light) noexcept
            : shape(shape), material(material), light(light)
    { }

    ShapePrimitive(
        const Frame& frame,
        const std::shared_ptr<Shape>& shape,
        const std::shared_ptr<Material>& material,
        const std::shared_ptr<DiffuseAreaLight>& light) noexcept
            : shape(std::make_shared<TransformedShape>(frame, shape))
            , material(material), light(light)
    { }

    Bounds3 getBounds() const override {
        return shape->getBounds();
    }

    const Material* getMaterial() const override {
        return material.get();
    }

    const DiffuseAreaLight* getAreaLight() const override {
        return light.get();
    }

    void computeScatteringFunctions(Interaction& isect) const override {
        if (material) material->computeScatteringFunctions(isect);
    }

    bool intersect(const Ray& ray, Interaction& isect) const override {
        Float tHit;
        if (!shape->intersect(ray, tHit, isect)) return false;
        ray.tMax = tHit;
        return true;
    }

    bool intersect(const Ray& ray) const override {
        return shape->intersect(ray);
    }

private:
    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
    std::shared_ptr<DiffuseAreaLight> light;
};

}

#endif
