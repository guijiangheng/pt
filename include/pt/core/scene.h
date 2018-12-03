#ifndef PT_CORE_SCENE_H
#define PT_CORE_SCENE_H

#include <memory>
#include <pt/core/primitive.h>

namespace pt {

class Scene {
public:
    Scene(std::unique_ptr<Primitive>&& aggregate)
        : aggregate(std::move(aggregate))
    { }

    bool intersect(const Ray& ray, Interaction& isect) const {
        return aggregate->intersect(ray, isect);
    }

    bool intersect(const Ray& ray) const {
        return aggregate->intersect(ray);
    }

private:
    std::unique_ptr<Primitive> aggregate;
};

}

#endif