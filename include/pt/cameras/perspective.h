#ifndef PT_CAMERAS_PERSPECTIVE_H
#define PT_CAMERAS_PERSPECTIVE_H

#include <pt/core/camera.h>

namespace pt {

class PerspectiveCamera : public ProjectiveCamera {
public:
    PerspectiveCamera(const Frame& frame, Film& film,
                      const Bounds2f& screenWindow,
                      Float lensRadius, Float focalDistance, Float fov)
        : ProjectiveCamera(
            frame, film,
            Matrix4::perspective(fov, 1e-2f, 1e4f),
            screenWindow, lensRadius, focalDistance)
    { }

    Ray generateRay(const CameraSample& sample) const override {
        Vector3 pFilm(sample.pFilm.x, sample.pFilm.y, 0);
        auto pCamera = rasterToCamera.applyP(pFilm);
        Ray ray(Vector3(0), normalize(pCamera));
        return frame.toWorld(ray);
    }
};

}

#endif