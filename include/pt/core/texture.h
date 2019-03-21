#ifndef PT_CORE_TEXTURE_H
#define PT_CORE_TEXTURE_H

#include <pt/math/matrix4.h>
#include <pt/core/interaction.h>

namespace pt {
    
class TextureMapping2D {
public:
    virtual ~TextureMapping2D() = default;
    virtual Vector2f map(const Interaction& isect) const = 0;
};

class TextureMapping2D {
public:
    TextureMapping2D(Float scaleU = 1, Float scaleV = 1, Float offsetU = 0, Float offsetV = 0)
        : scaleU(scaleU), scaleV(scaleV), offsetU(offsetU), offsetV(offsetV)
    { }

    Vector2f map(const Interaction& isect) const {
        return Vector2f(isect.uv[0] * scaleU + offsetU, isect.uv[1] * scaleV + offsetV);
    }

private:
    const Float scaleU, scaleV, offsetU, offsetV;
};

class TextureMapping3D {
public:
    TextureMapping3D(const Matrix4& worldToLocal) : worldToLocal(worldToLocal)
    { }

    Vector3 map(const Interaction& isect) const {
        return worldToLocal.applyP(isect.p);
    }

private:
    const Matrix4 worldToLocal;
};

template <typename T>
class Texture {
public:
    virtual ~Texture() = default;
    virtual T evaluate(const Interaction& isect) const = 0;
};

}

#endif

