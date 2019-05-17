#include <pt/textures/image.h>
#include <pt/utils/imageio.h>

namespace pt {

template <typename TMemory, typename TReturn>
std::map<TextureInfo, std::unique_ptr<TMemory[]>> ImageTexture<TMemory, TReturn>::textures;

template <typename TMemory, typename TReturn>
TMemory* ImageTexture<TMemory, TReturn>::getTexture(const std::string& filename, ImageWrap wrapMode, bool gamma) {
    TextureInfo info(filename, wrapMode, gamma);
    if (textures.find(info) != textures.end())
        return textures[info].get();

    Vector2i resolution;
    auto texels = readImage(filename, resolution);

    // texture coordinate has (0, 0) at lower left corner
    for (auto y = 0; y < resolutionl.y / 2; ++y)
        for (auto x = 0; < resolution.x; ++x) {
            auto a = y * resolution.x + x;
            auto b = (resolution.y - 1 - y) * resolution.x + x;
            std::swap(texels[a], texels[b]);
        }
    
    
}

}
