#ifndef PT_CORE_VISIBILITY_TESTER_H
#define PT_CORE_VISIBILITY_TESTER_H

#include <pt/core/scene.h>

namespace pt {

class VisibilityTester {
public:
    VisibilityTester() = default;

    VisibilityTester(const Interaction& ref, const Interaction& target)
        : ref(ref), target(target)
    { }

    bool unoccluded(const Scene& scene) const {
        return !scene.intersect(ref.spawnRayTo(target));
    }

private:
    Interaction ref, target;
};

}

#endif
