#include <random>
#include <vector>
#include <fstream>
#include <iostream>

#include <pt/core/film.h>
#include <pt/samplers/random.h>
#include <pt/shapes/sphere.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/core/scene.h>

using namespace pt;
using namespace std;

constexpr auto EPSILON = (Float)0.00001;

RandonSampler sampler(100);

Vector3 randomUnitSphere() {
    Vector3 p;
    do {
        p = Vector3(sampler.get1D(), sampler.get1D(), sampler.get1D());
        p = p * 2 - Vector3(1, 1, 1);
    } while (p.lengthSquared() > 1);
    return p;
}

Vector3 color(const Scene& scene, const Ray& ray) {
    Interaction isect;
    if (scene.intersect(ray, isect)) {
        Vector3 o = isect.p + isect.n * EPSILON;
        Vector3 target = isect.p + isect.n + randomUnitSphere();
        return color(scene, Ray(o, normalize(target - o))) * 0.5;
    }
    auto t = (ray.d.y + 1) / 2;
    return lerp(Vector3(1, 1, 1), Vector3(0.5, 0.7, 1.0), t);
}

int main() {
    constexpr auto width = 400;
    constexpr auto height = 200;

    vector<unique_ptr<Primitive>> primitives;
    primitives.emplace_back(make_unique<GeometricPrimitive>(Frame(), make_shared<Sphere>(0.5)));
    primitives.emplace_back(make_unique<GeometricPrimitive>(Frame::translate(0, -100.5, 0), make_shared<Sphere>(100)));
    Scene scene(std::make_unique<BVHAccel>(move(primitives)));

    Film film(
        Vector2i(width, height),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::translate(0, 0, -1), film,
        Bounds2f(Vector2f(-2, -1), Vector2f(2, 1)),
        0, 0, 90
    );

    for (auto p : film.pixelBounds) {
        Vector3 c(0, 0, 0);
        sampler.startPixel(p);
        do {
            auto cameraSample = sampler.getCameraSample(p);
            auto ray = camera.generateRay(cameraSample);
            c += color(scene, ray);
        } while (sampler.startNextSample());
        film.pixels.emplace_back(c / sampler.samplesPerPixel);
    }

    film.writeImage("./image.pfm");

    return 0;
}