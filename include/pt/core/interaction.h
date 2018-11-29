#ifndef PT_CORE_INTERACTION_H
#define PT_CORE_INTERACTION_H

#include <pt/math/vector3.h>

namespace pt {

class Interaction {
public:
    Interaction() = default;

    Interaction(const Vector3& p, const Vector3& n, const Vector3& wo)
        : p(p), n(n), wo(wo)
    { }

public:
    Vector3 p;
    Vector3 n;
    Vector3 wo;
};

}

#endif