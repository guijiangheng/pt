#include <pt/utils/imageio.h>

int main() {
    pt::Vector2i resolution;
    auto rgbs = pt::readImage("./image.png", resolution);
    auto outputBounds = pt::Bounds2i(pt::Vector2i(0), resolution);
    pt::writeImage("./out.png", &rgbs[0].x, outputBounds, resolution);
    auto exrRgbs = pt::readImage("./image.exr", resolution);
    outputBounds = pt::Bounds2i(pt::Vector2i(0), resolution);
    pt::writeImage("./image-write.exr", &exrRgbs[0].x, outputBounds, resolution);
}