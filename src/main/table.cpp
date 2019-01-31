#include <pt/utils/objloader.h>
#include <pt/core/scene.h>
#include <pt/materials/matte.h>
#include <pt/lights/diffuse.h>
#include <pt/materials/glass.h>
#include <pt/accelerators/bvh.h>
#include <pt/cameras/perspective.h>
#include <pt/samplers/random.h>
#include <pt/integrators/path.h>

using namespace pt;

int main() {
    std::vector<Primitive*> prims;
    std::vector<std::shared_ptr<Light>> lights;

    auto mesh = loadObjMesh("../assets/table/mesh_1.obj");
    auto lightMesh1 = Mesh(Frame::translate(10, 0, -25) * Frame::scale(0.06, 0.06, -1), mesh);
    auto triangles = createTriangleMesh(lightMesh1);
    for (auto& triangle : triangles) {
        auto light = std::make_shared<DiffuseAreaLight>(triangle, Vector3(3, 3, 2.5));
        lights.push_back(light);
        prims.push_back(new ShapePrimitive(triangle, nullptr, light));
    }

    auto lightMesh2 = Mesh(Frame::translate(0, 0, -60) * Frame::scale(0.3, 0.3, -1), mesh);
    triangles = createTriangleMesh(lightMesh2);
    for (auto& triangle : triangles) {
        auto light = std::make_shared<DiffuseAreaLight>(triangle, Vector3(1, 1, 1.6));
        lights.push_back(light);
        prims.push_back(new ShapePrimitive(triangle, nullptr, light));
    }

    auto plateMesh = Mesh(
        Frame::translate(3, 0, 0),
        loadObjMesh("../assets/table/mesh_0.obj")
    );
    auto plateMaterial = std::make_shared<MatteMaterial>(Vector3(1, 0, 0));
    triangles = createTriangleMesh(plateMesh);
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, plateMaterial));


    auto floorMesh = Mesh(
        Frame::translate(-35, 25, 0) * Frame::scale(0.2, 0.35, 0.5),
        mesh
    );
    triangles = createTriangleMesh(floorMesh);
    auto floorMaterial = std::make_shared<MatteMaterial>(Vector3(0.5, 0.5, 0.5));
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, floorMaterial));

    auto glass1Mesh = Mesh(
        Frame::translate(-1, 0, 0),
        loadObjMesh("../assets/table/mesh_2.obj")
    );
    triangles = createTriangleMesh(glass1Mesh);
    auto glass1Material = std::make_shared<GlassMaterial>(Vector3(1), Vector3(1), 1.33);
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, glass1Material));

    auto glass2Mesh = Mesh(
        Frame::translate(-1, 0, 0),
        loadObjMesh("../assets/table/mesh_3.obj")
    );
    triangles = createTriangleMesh(glass2Mesh);
    auto glass2Material = std::make_shared<GlassMaterial>(Vector3(1), Vector3(1), 1.5);
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, glass2Material));
    
    auto glass3Mesh = Mesh(
        Frame::translate(-1, 0, 0),
        loadObjMesh("../assets/table/mesh_4.obj")
    );
    triangles = createTriangleMesh(glass3Mesh);
    auto glass3Material = std::make_shared<GlassMaterial>(Vector3(1), Vector3(1), 0.8866667);
    for (auto& triangle : triangles)
        prims.push_back(new ShapePrimitive(triangle, glass3Material));
    
    BVHAccel accel(std::move(prims));
    Scene scene(accel, std::move(lights));
    Film film(
        Vector2i(800, 600),
        Bounds2f(Vector2f(0, 0), Vector2f(1, 1))
    );
    PerspectiveCamera camera(
        Frame::lookAt(
            Vector3(32.1259, -68.0505, -36.597),
            Vector3(31.6866, -67.2776, -36.1392),
            Vector3(-0.22886, 0.39656, -0.889024)
        ),
        film,
        Bounds2f(Vector2f(-1, -0.75), Vector2f(1, 0.75)),
        0, 0, 35
    );

    RandonSampler sampler(512);
    PathIntegrator integrator(20, camera, sampler);
    integrator.render(scene);
    film.writeImage("./image.pfm");

    return 0;
}
