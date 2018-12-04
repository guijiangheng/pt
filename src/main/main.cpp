#include <random>
#include <vector>
#include <fstream>
#include <iostream>

#include <pt/samplers/random.h>
#include <pt/shapes/sphere.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/core/scene.h>
#include <pt/core/integrator.h>

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

class SimpleIntegrator : public SamplerIntegrator {
public:
    SimpleIntegrator(Camera& camera, Sampler& sampler)
        : SamplerIntegrator(camera, sampler)
    { }

    Vector3 Li(const Ray& ray, const Scene& scene) const override {
        Interaction isect;
        if (scene.intersect(ray, isect)) {
            Vector3 o = isect.p + isect.n * EPSILON;
            Vector3 target = isect.p + isect.n + randomUnitSphere();
            return Li(Ray(o, normalize(target - o)), scene) * 0.5;
        }
        auto t = (ray.d.y + 1) / 2;
        return lerp(Vector3(1, 1, 1), Vector3(0.5, 0.7, 1.0), t);
    }
};

int main() {
    vector<Primitive*> primitives;
    primitives.push_back(new GeometricPrimitive(Frame(), make_shared<Sphere>(0.5)));
    primitives.push_back(new GeometricPrimitive(Frame::translate(0, -100.5, 0), make_shared<Sphere>(100)));
    BVHAccel accel(std::move(primitives));
    Scene scene(accel);

    Film film(
        Vector2i(400, 200),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::translate(0, 0, -1), film,
        Bounds2f(Vector2f(-2, -1), Vector2f(2, 1)),
        0, 0, 90
    );

    SimpleIntegrator integrator(camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}