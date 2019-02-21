#include <pt/core/scene.h>
#include <pt/core/integrator.h>
#include <pt/samplers/random.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/shapes/triangle.h>
#include <pt/utils/objloader.h>

using namespace pt;

class NormalIntegrator : public SamplerIntegrator {
public:
    NormalIntegrator(Camera& camera, Sampler& sampler)
        : SamplerIntegrator(camera, sampler)
    { }

    Vector3 li(const Ray& ray, const Scene& scene) const override {
        Interaction isect;
        if (scene.intersect(ray, isect))
            return abs(isect.n);
        return Vector3(0);
    }
};

int main() {
    auto ajax = loadObjMesh("../assets/ajax.obj");
    auto triangles = createTriangleMesh(ajax);
    std::vector<Primitive*> primitives;

    for (auto& triangle : triangles) {
        primitives.push_back(new ShapePrimitive(triangle));
    }

    BVHAccel accel(std::move(primitives));
    Scene scene(accel);

    Film film(
        Vector2i(512, 512),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::lookAt(
            Vector3(-65.6055, 47.5762, -24.3583),
            Vector3(-64.8161, 47.2211, -23.8576),
            Vector3(0.299858, 0.934836, 0.190177)
        ),
        film,
        Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
        0, 0, 30
    );

    RandonSampler sampler(1);
    NormalIntegrator integrator(camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
