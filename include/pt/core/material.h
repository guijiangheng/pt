#ifndef PT_CORE_MATERIAL_H
#define PT_CORE_MATERIAL_H

#include <pt/core/interaction.h>

namespace pt {

class Material {
public:
    Material() = default;
    virtual ~Material() = default;
    virtual void computeScatteringFunctions(Interaction& isect) const = 0;
};

}

#endif
