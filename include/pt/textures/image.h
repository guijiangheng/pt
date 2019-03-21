#ifndef PT_TEXTURES_IMAGE_H
#define PT_TEXTURES_IMAGE_H

#include <string>
#include <pt/core/texture.h>

namespace pt {

enum class ImageWrap { BLACK, CLAMP, REPEAT, MIRROR };

template <typename TMemory, typename TReturn>
class ImageTexture : public Texture<TReturn> {
public:
    ImageTexture(const TextureMapping2D& texutreMapping, const std::string& filename)
};

}

#endif
