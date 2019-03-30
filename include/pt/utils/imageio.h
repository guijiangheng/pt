#ifndef PT_UTILS_IMAGEIO_H
#define PT_UTILS_IMAGEIO_H

#include <string>
#include <pt/math/bounds2.h>

namespace pt {

void writeImage(
    const std::string& filename, const Float* rgbs,
    const Bounds2i& outputBounds, const Vector2i& totalResolution);

}

#endif
