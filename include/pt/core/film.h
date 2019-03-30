#ifndef PT_CORE_FILM_H
#define PT_CORE_FILM_H

#include <mutex>
#include <memory>
#include <pt/math/vector3.h>
#include <pt/math/bounds2.h>
#include <pt/core/filter.h>
#include <pt/utils/imageio.h>


namespace pt {

struct Pixel {
    Pixel() : color(0), filterWeight(0) { }
    Vector3 color;
    Float filterWeight;
};

class FilmTile {
public:
    FilmTile(const Bounds2i& pixelBounds, Float filterRadius,
             const Float* filterTable, int filterTableWidth)
        : pixelBounds(pixelBounds)
        , filterTable(filterTable)
        , filterRadius(filterRadius)
        , invFilterRadius(1 / filterRadius)
        , filterTableWidth(filterTableWidth) {
            
        pixels = std::unique_ptr<Pixel[]>(new Pixel[pixelBounds.area()]);
    }

    void addSample(const Vector2f& pFilm, const Vector3& color) {
        auto bounds = (Bounds2i)Bounds2f(
             ceil(pFilm - Vector2f(0.5) - Vector2f(filterRadius)),
            floor(pFilm + Vector2f(0.5) + Vector2f(filterRadius))
        );
        bounds = intersect(bounds, pixelBounds);
        for (auto p : bounds) {
            auto& pixel = getPixel(p);
            auto dist = pFilm - ((Vector2f)p + Vector2f(0.5));
            auto offset = (Vector2i)floor(abs(dist * invFilterRadius * filterTableWidth));
            auto filterWeight = filterTable[std::min(offset.x, filterTableWidth)] *
                                filterTable[std::min(offset.y, filterTableWidth)];
            pixel.filterWeight += filterWeight;
            pixel.color += color * filterWeight;
        }
    }

    Pixel& getPixel(const Vector2i& p) {
        auto width = pixelBounds.pMax.x - pixelBounds.pMin.x;
        auto offset = (p.x - pixelBounds.pMin.x) +
                      (p.y - pixelBounds.pMin.y) * width;
        return pixels[offset];
    }

    const Bounds2i& getPixelBounds() const {
        return pixelBounds;
    }

private:
    const Bounds2i pixelBounds;
    const Float* filterTable;
    Float filterRadius, invFilterRadius;
    int filterTableWidth;
    std::unique_ptr<Pixel[]> pixels;
};

class Film {
public:
    Film(const Vector2i& resolution, const Bounds2f& cropWindow, std::unique_ptr<Filter>&& _filter)
        : resolution(resolution)
        , pixelBounds(
            Vector2i(std::ceil(resolution.x * cropWindow.pMin.x), std::ceil(resolution.y * cropWindow.pMin.y)),
            Vector2i(std::ceil(resolution.x * cropWindow.pMax.x), std::ceil(resolution.y * cropWindow.pMax.y)))
        , filter(std::move(_filter)) {
            
        pixels = std::unique_ptr<Pixel[]>(new Pixel[pixelBounds.area()]);
        for (auto i = 0; i < filterTableWidth; ++i)
            filterTable[i] = filter->evaluate((i + (Float)0.5) * filter->radius / filterTableWidth);
    }

    Bounds2i getSampleBounds() const {
        return (Bounds2i)Bounds2f(
            floor((Vector2f)pixelBounds.pMin + Vector2f(0.5) - Vector2f(filter->radius)),
             ceil((Vector2f)pixelBounds.pMax - Vector2f(0.5) + Vector2f(filter->radius))
        );
    }

    std::unique_ptr<FilmTile> getFilmTile(const Bounds2i& sampleBounds) const {
        auto bounds = (Bounds2i)Bounds2f(
             ceil((Vector2f)sampleBounds.pMin - Vector2f(0.5) - Vector2f(filter->radius)),
            floor((Vector2f)sampleBounds.pMax + Vector2f(0.5) + Vector2f(filter->radius))
        );
        return std::unique_ptr<FilmTile>(new FilmTile(
            intersect(bounds, pixelBounds),
            filter->radius, filterTable, filterTableWidth
        ));
    }

    void mergeFilmTile(std::unique_ptr<FilmTile>&& tile) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& pixel : tile->getPixelBounds()) {
            auto& tilePixel = tile->getPixel(pixel);
            auto& filmPixel = getPixel(pixel);
            filmPixel.color += tilePixel.color;
            filmPixel.filterWeight += tilePixel.filterWeight;
        }
    }

    Pixel& getPixel(const Vector2i& p) {
        auto width = pixelBounds.pMax.x - pixelBounds.pMin.x;
        auto offset = (p.x - pixelBounds.pMin.x) +
                      (p.y - pixelBounds.pMin.y) * width;
        return pixels[offset];
    }

    void writeImage(const std::string& filename) {
        auto offset = 0;
        std::unique_ptr<Float[]> rgbs(new Float[3 * pixelBounds.area()]);
        for (auto p : pixelBounds) {
            auto& pixel = getPixel(p);
            auto rgb = pixel.color / pixel.filterWeight;
            rgbs[offset++] = rgb.x;
            rgbs[offset++] = rgb.y;
            rgbs[offset++] = rgb.z;
        }
        pt::writeImage(filename, &rgbs[0], pixelBounds, resolution);
    }

public:
    const Vector2i resolution;
    const Bounds2i pixelBounds;

private:
    std::mutex mutex;
    std::unique_ptr<Filter> filter;
    std::unique_ptr<Pixel[]> pixels;
    static constexpr int filterTableWidth = 16;
    Float filterTable[filterTableWidth];
};

}

#endif
