#include <pt/core/scene.h>
#include <pt/core/integrator.h>
#include <pt/samplers/random.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/shapes/sphere.h>

using namespace pt;

class NormalIntegrator : public SamplerIntegrator {
public:
    NormalIntegrator(Camera& camera, Sampler& sampler)
        : SamplerIntegrator(camera, sampler)
    { }

    Vector3 Li(const Ray& ray, const Scene& scene) const override {
        Interaction isect;
        if (scene.intersect(ray, isect))
            return abs(isect.n);
        return Vector3(0);
    }
};

int main() {
    std::vector<Primitive*> prims;
    auto sphere = std::make_shared<Sphere>(1);
    prims.push_back(new ShapePrimitive(Frame::translate(-1, 0, 0), sphere));
    prims.push_back(new ShapePrimitive(Frame::translate( 1, 0, 0), sphere));

    BVHAccel accel(std::move(prims));
    Scene scene(accel);

    Film film(
        Vector2i(512, 512),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::lookAt(
            Vector3(0, 0, -30),
            Vector3(0),
            Vector3(0, 1, 0)
        ), film,
        Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
        0, 0, 16
    );

    RandonSampler sampler(1);
    NormalIntegrator integrator(camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}