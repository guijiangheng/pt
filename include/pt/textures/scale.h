#ifndef PT_TEXTURES_SCALE_H
#define PT_TEXTURES_SCALE_H

#include <memory>
#include <pt/core/texture.h>

namespace pt {

template <typename T1, typename T2>
class ScaleTexture : public Texture<T2> {
public:
    ScaleTexture(const std::shared_ptr<Texture<T1>>& texture1,
                 const std::shared_ptr<Texture<T2>>& texture2)
        : texture1(texture1), texture2(texture2)
    { }

    T2 evaluate(const Interaction& isect) const override {
        return texture2->evaluate(isect) * texture1->evaluate(isect);
    }

private:
    std::shared_ptr<Texture<T1>> texture1;
    std::shared_ptr<Texture<T2>> texture2;
};

}

#endif

