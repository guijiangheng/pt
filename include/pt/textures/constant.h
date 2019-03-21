#ifndef PT_TEXTURES_CONSTANT_H
#define PT_TEXTURES_CONSTANT_H

#include <pt/core/texture.h>

namespace pt {

template <typename T>
class ConstantTexture : public Texture {
public:
    ConstantTexture(const T& value) : value(value)
    { }

    T evaluate(const Interaction& isect) const override {
        return value;
    }

private:
    const T value;
};

}

#endif

