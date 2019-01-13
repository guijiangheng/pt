#ifndef PT_CORE_SCENE_H
#define PT_CORE_SCENE_H

#include <vector>
#include <pt/core/light.h>
#include <pt/core/primitive.h>

namespace pt {

class Scene {
public:
    Scene(const Primitive& aggregate, std::vector<Light*>&& lights) noexcept
        : aggregate(aggregate)
        , lights(std::move(lights))
        , infiniteLight(nullptr) {
        
        // only one infinite area light is supported
        for (auto light : lights) {
            if (light->flags == LightFlags::Infinite) {
                infiniteLight = light;
                break;
            }
        }
    }

    ~Scene() {
        for (auto light : lights) delete light;
    }

    bool intersect(const Ray& ray, Interaction& isect) const {
        return aggregate.intersect(ray, isect);
    }

    bool intersect(const Ray& ray) const {
        return aggregate.intersect(ray);
    }

public:
    const Primitive& aggregate;
    std::vector<Light*> lights;
    Light* infiniteLight;
};

}

#endif
