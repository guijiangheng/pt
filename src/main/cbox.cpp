#include <pt/utils/objloader.h>
#include <pt/core/scene.h>
#include <pt/materials/matte.h>
#include <pt/lights/diffuse.h>
#include <pt/accelerators/bvh.h>
#include <pt/cameras/perspective.h>
#include <pt/samplers/random.h>
#include <pt/integrators/path.h>

using namespace pt;

class NormalIntegrator : public SamplerIntegrator {
public:
    NormalIntegrator(Camera& camera, Sampler& sampler)
        : SamplerIntegrator(camera, sampler)
    { }

    Vector3 li(const Ray& ray, const Scene& scene) const override {
        Interaction isect;
        if (scene.intersect(ray, isect))
            return max(isect.n, Vector3(0));
        return Vector3(0);
    }
};

int main() {
    std::vector<Primitive*> prims;
    std::vector<std::shared_ptr<Light>> lights;

    auto walls = loadObjMesh("../assets/walls.obj");
    auto triangles = createTriangleMesh(walls);
    auto wallMaterial = std::make_shared<MatteMaterial>(Vector3(0.725, 0.71, 0.68));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, wallMaterial));

    auto rightWall = loadObjMesh("../assets/rightwall.obj");
    triangles = createTriangleMesh(rightWall);
    auto rightWallMaterial = std::make_shared<MatteMaterial>(Vector3(0.161, 0.133, 0.427));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, rightWallMaterial));
    
    auto leftWall = loadObjMesh("../assets/leftwall.obj");
    triangles = createTriangleMesh(leftWall);
    auto leftWallMaterial = std::make_shared<MatteMaterial>(Vector3(0.630, 0.065, 0.05));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, leftWallMaterial));

    auto sphere1 = loadObjMesh("../assets/sphere1.obj");
    triangles = createTriangleMesh(sphere1);
    auto sphere1Material = std::make_shared<MatteMaterial>(Vector3(0.630, 0.065, 0.05));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, sphere1Material));
    
    auto sphere2 = loadObjMesh("../assets/sphere2.obj");
    triangles = createTriangleMesh(sphere2);
    auto sphere2Material = std::make_shared<MatteMaterial>(Vector3(0.161, 0.133, 0.427));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, sphere2Material));
    
    auto light = loadObjMesh("../assets/light.obj");
    triangles = createTriangleMesh(light);
    for (auto& triangle : triangles) {
        auto light = std::make_shared<DiffuseAreaLight>(triangle, Vector3(10));
        lights.push_back(light);
        prims.push_back(new ShapePrimitive(triangle, nullptr, light));
    }

    BVHAccel accel(std::move(prims));
    Scene scene(accel, std::move(lights));
    Film film(
        Vector2i(800, 600),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );
    PerspectiveCamera camera(
        Frame::scale(-1, 1, 1) * Frame::lookAt(
            Vector3(0, 0.919769, -5.41159),
            Vector3(0, 0.893051, -4.41198),
            Vector3(0, 1, 0)
        ),
        film,
        Bounds2f(Vector2f(-1, -0.75), Vector2f(1, 0.75)),
        0, 0, 27.7856
    );

    RandonSampler sampler(64);
    PathIntegrator integrator(5, camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
