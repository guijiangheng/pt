#include <pt/utils/objloader.h>
#include <pt/core/scene.h>
#include <pt/materials/matte.h>
#include <pt/materials/glass.h>
#include <pt/materials/mirror.h>
#include <pt/lights/diffuse.h>
#include <pt/accelerators/bvh.h>
#include <pt/cameras/perspective.h>
#include <pt/samplers/random.h>
#include <pt/integrators/path.h>

using namespace pt;

int main() {
    std::vector<Primitive*> prims;
    std::vector<std::shared_ptr<Light>> lights;

    auto walls = loadObjMesh("../assets/walls.obj");
    auto triangles = createTriangleMesh(walls);
    auto wallMaterial = std::make_shared<MatteMaterial>(Vector3(0.725, 0.71, 0.68));
    for (auto& triangle : triangles)
        prims.push_back(new GeometricPrimitive(triangle, wallMaterial));

    auto rightWall = loadObjMesh("../assets/rightwall.obj");
    triangles = createTriangleMesh(rightWall);
    auto rightWallMaterial = std::make_shared<MatteMaterial>(Vector3(0.161, 0.133, 0.427));
    for (auto& triangle : triangles)
        prims.push_back(new GeometricPrimitive(triangle, rightWallMaterial));
    
    auto leftWall = loadObjMesh("../assets/leftwall.obj");
    triangles = createTriangleMesh(leftWall);
    auto leftWallMaterial = std::make_shared<MatteMaterial>(Vector3(0.630, 0.065, 0.05));
    for (auto& triangle : triangles)
        prims.push_back(new GeometricPrimitive(triangle, leftWallMaterial));

    auto sphere1 = loadObjMesh("../assets/sphere1.obj");
    triangles = createTriangleMesh(sphere1);
    auto sphere1Material = std::make_shared<MirrorMaterial>(Vector3(1));
    for (auto& triangle : triangles)
        prims.push_back(new GeometricPrimitive(triangle, sphere1Material));
    
    auto sphere2 = loadObjMesh("../assets/sphere2.obj");
    triangles = createTriangleMesh(sphere2);
    auto sphere2Material = std::make_shared<GlassMaterial>(Vector3(1), Vector3(1), 1.4);
    for (auto& triangle : triangles)
        prims.push_back(new GeometricPrimitive(triangle, sphere2Material));
    
    auto light = loadObjMesh("../assets/light.obj");
    triangles = createTriangleMesh(light);
    for (auto& triangle : triangles) {
        auto light = std::make_shared<DiffuseAreaLight>(triangle, Vector3(6.36));
        lights.push_back(light);
        prims.push_back(new GeometricPrimitive(triangle, nullptr, light));
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

    RandonSampler sampler(4);
    PathIntegrator integrator(10, camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
