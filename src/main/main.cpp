#include <random>
#include <vector>
#include <fstream>
#include <iostream>

#include <pt/core/film.h>
#include <pt/core/primitive.h>
#include <pt/shapes/sphere.h>
#include <pt/camera/perspective.h>

using namespace pt;
using namespace std;

constexpr auto EPSILON = (Float)0.00001;

random_device dev;
mt19937 gen;
uniform_real_distribution<Float> d;

Vector3 randomUnitSphere() {
    Vector3 p;
    do {
        p = Vector3(d(gen), d(gen), d(gen)) * 2 - Vector3(1, 1, 1);
    } while (p.lengthSquared() > 1);
    return p;
}

Vector3 color(const vector<GeometricPrimitive>& scene, const Ray& r) {
    bool hit = false;
    Interaction isect;

    for (auto obj : scene) {
        if (obj.intersect(r, isect))
            hit = true;
    }

    if (hit) {
        Vector3 o = isect.p + isect.n * EPSILON;
        Vector3 target = isect.p + isect.n + randomUnitSphere();
        return color(scene, Ray(o, normalize(target - o))) * 0.5;
    }

    auto t = (r.d.y + 1) / 2;
    return lerp(Vector3(1, 1, 1), Vector3(0.5, 0.7, 1.0), t);
}

int main() {
    constexpr auto width = 400;
    constexpr auto height = 200;
    constexpr auto samples = 100;
    
    vector<GeometricPrimitive> scene;
    GeometricPrimitive a(Frame(), std::make_shared<Sphere>(0.5));
    GeometricPrimitive b(Frame::translate(0, -100.5, 0), std::make_shared<Sphere>(100));
    scene.push_back(a);
    scene.push_back(b);

    random_device dev;
    mt19937 gen(dev());
    std::uniform_real_distribution<Float> d(0, 1);

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
        for (auto s = 0; s < samples; ++s) {
            CameraSample sample;
            sample.pFilm = Vector2f(p.x, p.y) + Vector2f(d(gen), d(gen));
            auto ray = camera.generateRay(sample);
            c += color(scene, ray);
        }
        film.pixels.emplace_back(c / samples);
    }

    film.writeImage("./image.pfm");

    return 0;
}