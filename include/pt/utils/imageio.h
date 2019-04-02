#ifndef PT_UTILS_IMAGEIO_H
#define PT_UTILS_IMAGEIO_H

#include <string>
#include <memory>
#include <pt/math/bounds2.h>
#include <pt/math/vector3.h>

namespace pt {

std::unique_ptr<Vector3[]> readImage(const std::string& filename, Vector2i& resolution);

void writeImage(
    const std::string& filename, const Float* rgbs,
    const Bounds2i& outputBounds, const Vector2i& totalResolution);

}

#endif
