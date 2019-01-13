#ifndef PT_CORE_SCENE_H
#define PT_CORE_SCENE_H

#include <vector>
#include <pt/core/primitive.h>

namespace pt {

class Scene {
public:
    Scene(const Primitive& accel, std::vector<Light*>&& lights = std::vector<Light*>()) noexcept
        : accel(accel)
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

    ~Scene() noexcept {
        for (auto light : lights) delete light;
    }

    bool intersect(const Ray& ray, Interaction& isect) const {
        return accel.intersect(ray, isect);
    }

    bool intersect(const Ray& ray) const {
        return accel.intersect(ray);
    }

public:
    const Primitive& accel;
    std::vector<Light*> lights;
    Light* infiniteLight;
};

}

#endif
