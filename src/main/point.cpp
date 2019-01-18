#include <pt/utils/objloader.h>
#include <pt/core/film.h>
#include <pt/core/scene.h>
#include <pt/materials/matte.h>
#include <pt/accelerators/bvh.h>
#include <pt/lights/point.h>
#include <pt/cameras/perspective.h>
#include <pt/samplers/random.h>
#include <pt/integrators/path.h>

using namespace pt;
using namespace std;

int main() {
    auto matte = make_shared<MatteMaterial>(Vector3(1));
    auto ajax = loadObjMesh("../assets/ajax.obj");
    auto triangles = createTriangleMesh(ajax);
    std::vector<Primitive*> primitives;

    for (auto& triangle : triangles)
        primitives.push_back(new ShapePrimitive(triangle, matte));

    BVHAccel accel(std::move(primitives));

    std::vector<std::shared_ptr<Light>> lights;
    lights.push_back(std::make_shared<PointLight>(Vector3(-20, 40, -20), Vector3(2995)));

    Scene scene(accel, std::move(lights));

    Film film(
        Vector2i(800, 800),
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

    RandonSampler sampler(32);
    PathIntegrator integrator(1, camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
