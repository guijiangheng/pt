#include <pt/core/scene.h>
#include <pt/core/visibilitytester.h>

namespace pt {

// Scene class's dependency is too much
// implement this simple method in cpp file to avoid circular dependency 
bool VisibilityTester::unoccluded(const Scene& scene) const {
    return !scene.intersect(ref.spawnRayTo(target));
}

}
