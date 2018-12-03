#ifndef PT_CORE_INTEGRATOR_H
#define PT_CORE_INTEGRATOR_H

#include <pt/core/scene.h>

namespace pt {

class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void render(const Scene& scene) = 0;
};

}

#endif