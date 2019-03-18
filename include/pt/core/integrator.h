#ifndef PT_CORE_INTEGRATOR_H
#define PT_CORE_INTEGRATOR_H

#include <pt/core/scene.h>
#include <pt/core/camera.h>
#include <pt/core/sampler.h>
#include <pt/core/parallel.h>

namespace pt {

class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void render(const Scene& scene) = 0;
};

class SamplerIntegrator : public Integrator {
public:
    SamplerIntegrator(Camera& camera, Sampler& sampler) noexcept
        : camera(camera), sampler(sampler)
    { }

    virtual Vector3 li(const Ray& ray, const Scene& scene) const = 0;

    void render(const Scene& scene) override {
        parallelInit();

        auto sampleBounds = camera.film.getSampleBounds();
        auto diag = sampleBounds.diag();
        Vector2i nTiles((diag.x + TileSize - 1) / TileSize,
                        (diag.y + TileSize - 1) / TileSize);

        parallelFor2D([&](const Vector2i& tile) {
            auto x0 = sampleBounds.pMin.x + TileSize * tile.x;
            auto x1 = std::min(sampleBounds.pMax.x, x0 + TileSize);
            auto y0 = sampleBounds.pMin.y + TileSize * tile.y;
            auto y1 = std::min(sampleBounds.pMax.y, y0 + TileSize);
            Bounds2i tileBounds(Vector2i(x0, y0), Vector2i(x1, y1));
            auto filmTile = camera.film.getFilmTile(tileBounds);
            auto seed = tile.y * nTiles.x + tile.x;
            auto tileSampler = sampler.clone(seed);
            for (auto p : tileBounds) {
                Vector3 color(0);
                tileSampler->startPixel();
                do {
                    auto cameraSample = tileSampler->getCameraSample(p);
                    auto ray = camera.generateRay(cameraSample);
                    filmTile->addSample(cameraSample.pFilm, li(ray, scene));
                } while (tileSampler->startNextSample());
            }
            camera.film.mergeFilmTile(std::move(filmTile));
        }, nTiles);

        parallelCleanup();
    }

protected:
    Camera& camera;
    Sampler& sampler;
};

}

#endif
