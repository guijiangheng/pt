#ifndef PT_TEXTURES_MIX_H
#define PT_TEXTURES_MIX_H

#include <memory>
#include <pt/core/texture.h>

namespace pt {

template <typename T>
class MixTexture : public Texture<T> {
public:
    MixTexture(const std::shared_ptr<Texture<T>>& texture1,
               const std::shared_ptr<Texture<T>>& texture2,
               const std::shared_ptr<Texture<Float>>& mix)
        : texture1(texture1), texture2(texture2), mix(mix)
    { }

    T evaluate(const Interaction& isect) const override {
        auto t1 = texture1->evaluate(isect);
        auto t2 = texture2->evaluate(isect);
        return t1 + (t2 - t1) * mix->evaluate(isect);
    }

private:
    std::shared_ptr<Texture> texture1, texture2;
    std::shared_ptr<Float> mix;
};

}

#endif

