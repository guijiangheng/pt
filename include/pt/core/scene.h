#ifndef PT_CORE_SCENE_H
#define PT_CORE_SCENE_H

#include <vector>
#include <pt/core/primitive.h>

namespace pt {

class Scene {
public:
    Scene(
        const Primitive& accel,
        std::vector<std::shared_ptr<Light>>&& lights = std::vector<std::shared_ptr<Light>>()) noexcept
            : accel(accel)
            , lights(std::move(lights))
            , infiniteLight(nullptr) {
        
        // only one infinite area light is supported
        for (auto light : lights) {
            if (light->flags == LightFlags::Infinite) {
                infiniteLight = light.get();
                break;
            }
        }
    }

    bool intersect(const Ray& ray, Interaction& isect) const {
        return accel.intersect(ray, isect);
    }

    bool intersect(const Ray& ray) const {
        return accel.intersect(ray);
    }

public:
    const Primitive& accel;
    std::vector<std::shared_ptr<Light>> lights;
    Light* infiniteLight;
};

}

#endif
