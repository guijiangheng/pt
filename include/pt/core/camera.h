#ifndef PT_CORE_CAMERA_H
#define PT_CORE_CAMERA_H

#include <pt/core/film.h>
#include <pt/core/frame.h>
#include <pt/math/bounds2.h>

namespace pt {

struct CameraSample {
    Vector2f pFilm;
    Vector2f pLens;
};

class Camera {
public:
    Camera(const Frame& frame, Film& film) noexcept
        : frame(frame), film(film)
    { }

    virtual ~Camera() = default;

    virtual Ray generateRay(const CameraSample& sample) const = 0;

public:
    Frame frame;
    Film& film;
};

class ProjectiveCamera : public Camera {
public:
    ProjectiveCamera(
        const Frame& frame, Film& film,
        const Matrix4& cameraToScreen,
        const Bounds2f& screenWindow,
        Float lensRadius, Float focalDistance) noexcept
            : Camera(frame, film)
            , lensRadius(lensRadius), focalDistance(focalDistance) {
        
        auto screenToRaster = 
            Matrix4::scale(film.resolution.x, film.resolution.y, 1) *
            Matrix4::scale(
                1 / (screenWindow.pMax.x - screenWindow.pMin.x),
                1 / (screenWindow.pMin.y - screenWindow.pMax.y), 1) *
            Matrix4::translate(-screenWindow.pMin.x, -screenWindow.pMax.y, 0);
        cameraToRaster = screenToRaster * cameraToScreen;
        rasterToCamera = inverse(cameraToRaster);
    }

public:
    Matrix4 rasterToCamera, cameraToRaster;
    Float lensRadius, focalDistance;
};

}

#endif
