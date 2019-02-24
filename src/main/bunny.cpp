#include <pt/core/scene.h>
#include <pt/core/integrator.h>
#include <pt/samplers/random.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/shapes/triangle.h>
#include <pt/utils/objloader.h>

using namespace pt;
using namespace std;

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
    auto bunny = loadObjMesh("../assets/bunny.obj");
    auto triangles = createTriangleMesh(bunny);
    std::vector<Primitive*> primitives;

    for (auto& triangle : triangles) {
        primitives.push_back(new GeometricPrimitive(triangle));
    }

    BVHAccel accel(std::move(primitives));
    Scene scene(accel);

    Film film(
        Vector2i(512, 512),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::lookAt(
            Vector3(-0.0315182, 0.284011, -0.7331),
            Vector3(-0.0123771, 0.0540913, 0.239922),
            Vector3(0.00717446, 0.973206, 0.229822)
        ),
        film,
        Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
        0, 0, 16
    );

    RandonSampler sampler(1);
    NormalIntegrator integrator(camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
