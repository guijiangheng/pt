#ifndef PT_CORE_FILM_H
#define PT_CORE_FILM_H

#include <string>
#include <vector>
#include <fstream>

#include <pt/math/vector2.h>
#include <pt/math/vector3.h>
#include <pt/math/bounds2.h>

namespace pt {

class Film {
public:
    Film(const Vector2i& resolution, const Bounds2f& cropWindow)
        : resolution(resolution)
        , pixelBounds(
            Vector2i(std::ceil(resolution.x * cropWindow.pMin.x), std::ceil(resolution.y * cropWindow.pMin.y)),
            Vector2i(std::ceil(resolution.x * cropWindow.pMax.x), std::ceil(resolution.y * cropWindow.pMax.y))
        ) {

        pixels.reserve(pixelBounds.area());
    }

    void writeImage(const std::string& name) {
        std::fstream file(name, std::ios::out | std::ios::binary);
        auto diag = pixelBounds.diag();
        auto width = diag.x, height = diag.y;
        file << "PF\n" << width << " " << height << "\n-1\n";

        for (auto y = height - 1; y >= 0; --y)
            for (auto x = 0; x < width; ++x) {
                auto color = pixels[width * y + x];
                file.write((char*)&color, sizeof(Vector3));
            }

        file.close();
    }

public:
    const Vector2i resolution;
    const Bounds2i pixelBounds;
    std::vector<Vector3> pixels;
};

}

#endif
