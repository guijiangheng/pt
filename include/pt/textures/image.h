#ifndef PT_TEXTURES_IMAGE_H
#define PT_TEXTURES_IMAGE_H

#include <map>
#include <string>
#include <memory>
#include <pt/core/texture.h>

namespace pt {

enum class ImageWrap { BLACK, CLAMP, REPEAT, MIRROR };

struct TextureInfo {
    TextureInfo(const std::string& filename, ImageWrap wrapMode, bool gamma)
        : filename(filename), wrapMode(wrapMode), gamma(gamma)
    { }

    bool operator<(const TextureInfo& rhs) const {
        if (filename != rhs.filename) return filename < rhs.filename;
        if (wrapMode != rhs.wrapMode) return wrapMode < rhs.wrapMode;
        return gamma < rhs.gamma;
    }

    std::string filename;
    ImageWrap wrapMode;
    bool gamma;
};

template <typename TMemory, typename TReturn>
class ImageTexture : public Texture<TReturn> {
public:
    ImageTexture(const TextureMapping2D& mapping,
                 const std::string& filename, ImageWrap wrapMode, bool gamma)
        : mapping(mapping)
        , texture(getTexture(filename, wrapMode, gamma))
    { }

private:
    static TMemory* getTexture(const std::string& filename, ImageWrap wrapMode, bool gamma);

private:
    TextureMapping2D mapping;
    TMemory* texture;
    static std::map<TextureInfo, std::unique_ptr<TMemory[]>> textures;
};

}

#endif
