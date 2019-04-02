#include <fstream>
#include <experimental/filesystem>
#include <pt/math/math.h>
#include <pt/utils/imageio.h>
#include <lodepng/lodepng.h>
#include <ImfRgba.h>
#include <ImfRgbaFile.h>

namespace fs = std::experimental::filesystem;

namespace pt {

static Vector3* readImagePNG(const std::string& filename, Vector2i& resolution) {
    unsigned int width, height;
    std::vector<std::uint8_t> bytes;
    auto error = lodepng::decode(bytes, width, height, filename.c_str(), LCT_RGB);
    if (error) throw std::runtime_error("Failed to load png file: " + std::string(lodepng_error_text(error)));
    resolution = Vector2i((int)width, (int)height);
    auto offset = 0;
    auto rgbs = new Vector3[width * height * 3];
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x) {
            auto r = bytes[offset + 0] / (Float)255;
            auto g = bytes[offset + 1] / (Float)255;
            auto b = bytes[offset + 2] / (Float)255;
            rgbs[y * width + x] = Vector3(r, g, b);
            offset += 3;
        }
    return rgbs;
}

static Vector3* readImageEXR(const std::string& filename, Vector2i& resolution) {
    using namespace Imf;
    using namespace Imath;
    RgbaInputFile file(filename.c_str());
    auto dataWindow = file.dataWindow();
    auto width = dataWindow.max.x - dataWindow.min.x + 1;
    auto height = dataWindow.max.y - dataWindow.min.y + 1;
    resolution = Vector2i(width, height);
    std::vector<Rgba> pixels(width * height);
    file.setFrameBuffer(&pixels[0] - dataWindow.min.x - dataWindow.min.y * width, 1, width);
    file.readPixels(dataWindow.min.y, dataWindow.max.y);
    auto rgbs = new Vector3[width * height];
    for (auto i = 0; i < width * height; ++i)
        rgbs[i] = Vector3(pixels[i].r, pixels[i].g, pixels[i].b);
    return rgbs;
}

std::unique_ptr<Vector3[]> readImage(const std::string& filename, Vector2i& resolution) {
    auto extension = fs::path(filename).extension();
    if (extension == ".exr") {
        return std::unique_ptr<Vector3[]>(readImageEXR(filename, resolution));
    } else if (extension == ".png") {
        return std::unique_ptr<Vector3[]>(readImagePNG(filename, resolution));
    } else {
        throw std::runtime_error("Input image file format not supported!");
    }
}

static void writeImagePFM(const std::string& filename, const Float* rgbs, int width, int height) {
    std::fstream file(filename, std::ios::out | std::ios::binary);
    file << "PF\n" << width << " " << height << "\n-1\n";
    for (auto y = height - 1; y >= 0; --y)
        file.write((char*)(rgbs + y * width * 3), sizeof(Float) * width * 3);
    file.close();
}

static void writeImageEXR(const std::string& filename, const Float* rgbs,
                          const Bounds2i& outputBounds, const Vector2i& totalResolution) {
    using namespace Imf;
    using namespace Imath;
    auto diag = outputBounds.diag();
    auto rgbas = new Rgba[outputBounds.area()];
    for (auto i = 0; i < outputBounds.area(); ++i)
        rgbas[i] = Rgba(rgbs[3 * i], rgbs[3 * i + 1], rgbs[3 * i + 2]);
    Box2i displayWindow(V2i(0, 0), V2i(totalResolution.x - 1, totalResolution.y - 1));
    Box2i dataWindow(V2i(outputBounds.pMin.x, outputBounds.pMin.y),
                     V2i(outputBounds.pMax.x - 1, outputBounds.pMax.y - 1));
    RgbaOutputFile file(filename.c_str(), displayWindow, dataWindow, WRITE_RGB);
    file.setFrameBuffer(rgbas - outputBounds.pMin.x - outputBounds.pMin.y * diag.x, 1, diag.x);
    file.writePixels(diag.y);
    delete[] rgbas;
}

static void writeImagePNG(const std::string& filename, const Float* rgbs, int width, int height) {
    auto offset = 0;
    std::unique_ptr<std::uint8_t[]> bytes(new std::uint8_t[width * height * 3]);
    for (auto y = 0; y < height; ++y)
        for (auto x = 0; x < width; ++x) {
            bytes[offset + 0] = (std::uint8_t)std::min(255 * gammaCorrect(rgbs[offset + 0]), (Float)255);
            bytes[offset + 1] = (std::uint8_t)std::min(255 * gammaCorrect(rgbs[offset + 1]), (Float)255);
            bytes[offset + 2] = (std::uint8_t)std::min(255 * gammaCorrect(rgbs[offset + 2]), (Float)255);
            offset += 3;
        }
    lodepng::encode(filename, &bytes[0], width, height, LCT_RGB);
}

void writeImage(const std::string& filename, const Float* rgbs,
                const Bounds2i& outputBounds, const Vector2i& totalResolution) {
    auto extension = fs::path(filename).extension();
    auto diag = outputBounds.diag();
    if (extension == ".exr") {
        writeImageEXR(filename, rgbs, outputBounds, totalResolution);
    } else if (extension == ".pfm") {
        writeImagePFM(filename, rgbs, diag.x, diag.y);
    } else if (extension == ".png") {
        writeImagePNG(filename, rgbs, diag.x, diag.y);
    } else {
        throw std::runtime_error("Output image file format not supported!");
    }
}

}
