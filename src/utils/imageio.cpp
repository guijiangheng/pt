#include <fstream>
#include <experimental/filesystem>
#include <pt/utils/imageio.h>
#include <ImfRgba.h>
#include <ImfRgbaFile.h>

namespace fs = std::experimental::filesystem;

namespace pt {

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

void writeImage(const std::string& filename, const Float* rgbs,
                const Bounds2i& outputBounds, const Vector2i& totalResolution) {
    auto extension = fs::path(filename).extension();
    auto diag = outputBounds.diag();
    if (extension == ".exr") {
        writeImageEXR(filename, rgbs, outputBounds, totalResolution);
    } else if (extension == ".pfm") {
        writeImagePFM(filename, rgbs, diag.x, diag.y);
    } else {
        throw std::runtime_error("Output image file format not support!");
    }
}

}
