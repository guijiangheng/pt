#include <pt/core/scene.h>
#include <pt/core/integrator.h>
#include <pt/samplers/random.h>
#include <pt/cameras/perspective.h>
#include <pt/accelerators/bvh.h>
#include <pt/shapes/triangle.h>
#include <pt/utils/plyloader.h>

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
    auto dragon = Mesh(
        Frame::rotate(Vector3(0, 1, 0), -53),
        loadPLYMesh("../assets/dragon.ply")
    );
    auto triangles = createTriangleMesh(dragon);
    std::vector<Primitive*> primitives;

    for (auto& triangle : triangles) {
        primitives.push_back(new GeometricPrimitive(triangle));
    }

    BVHAccel accel(std::move(primitives));
    Scene scene(accel);

    Film film(
        Vector2i(800, 800),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );

    PerspectiveCamera camera(
        Frame::lookAt(
            Vector3(277, -240, 250),
            Vector3(0, 60, -30),
            Vector3(0, 0, 1)
        ),
        film,
        Bounds2f(Vector2f(-1, -1), Vector2f(1, 1)),
        0, 0, 30
    );

    RandomSampler sampler(1);
    NormalIntegrator integrator(camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
