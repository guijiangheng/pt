#ifndef PT_CORE_VISIBILITY_TESTER_H
#define PT_CORE_VISIBILITY_TESTER_H

#include <pt/core/interaction.h>

namespace pt {

class Scene;

class VisibilityTester {
public:
    VisibilityTester() = default;

    VisibilityTester(const Interaction& ref, const Interaction& target) noexcept
        : ref(ref), target(target)
    { }

    bool unoccluded(const Scene& scene) const;

private:
    Interaction ref, target;
};

}

#endif
