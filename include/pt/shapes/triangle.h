#ifndef PT_SHAPES_TRIANGLE_H
#define PT_SHAPES_TRIANGLE_H

#include <vector>
#include <pt/pt.h>
#include <pt/core/shape.h>

namespace pt {

class Mesh {
public:
    Mesh(std::vector<int>&& indices,
         std::vector<Vector3>&& vertices,
         std::vector<Vector3>&& normals = std::vector<Vector3>(),
         std::vector<Vector2f>&& uvs = std::vector<Vector2f>())
        : indices(indices)
        , vertices(vertices)
        , normals(normals)
        , uvs(uvs)
    { }

    Mesh(const Frame& frame, const Mesh& mesh) : indices(mesh.indices) {
        vertices.reserve(mesh.vertices.size());
        for (auto& p : mesh.vertices) {
            vertices.push_back(frame.toWorldP(p));
        }

        if (!mesh.normals.empty())  {
            normals.reserve(mesh.normals.size());
            for (auto& n : mesh.normals) {
                normals.push_back(frame.toWorldN(n));
            }
        }

        if (!mesh.uvs.empty()) {
            uvs = mesh.uvs;
        }
    }

public:
    std::vector<int> indices;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2f> uvs;
};

class Triangle : public Shape {
public:
    Triangle(const Mesh& mesh, int triangleIndex)
        : mesh(mesh), indices(&mesh.indices[3 * triangleIndex])
    { }

    Bounds3 getBounds() const override {
        auto& a = mesh.vertices[indices[0]];
        auto& b = mesh.vertices[indices[1]];
        auto& c = mesh.vertices[indices[2]];
        return merge(Bounds3(a, b), c);
    }

    // ref https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
    bool intersect(const Ray& ray, Float& tHit, Interaction& isect) const override {
        auto& a = mesh.vertices[indices[0]];
        auto& b = mesh.vertices[indices[1]];
        auto& c = mesh.vertices[indices[2]];

        auto edge1 = b - a;
        auto edge2 = c - a;
        auto p = cross(ray.d, edge2);
        auto det = dot(p, edge1);

        if (std::abs(det) < TriangleIntersctEpsilon)
            return false;

        auto t = ray.o - a;
        auto detInv = 1 / det;
        auto u = dot(p, t) * detInv;
        if (u < 0 || u > 1) return false;

        auto q = cross(t, edge1);
        auto v = dot(q, ray.d) * detInv;
        if (v < 0 || u + v > 1) return false;

        auto dist = dot(q, edge2) * detInv;
        if (dist <= 0 || dist > ray.tMax) return false;

        tHit = dist;
        isect.p = a + edge1 * u + edge2 * v;
        isect.n = normalize(cross(edge2, edge1));
        isect.wo = -ray.d;

        return true;
    }

public:
    const Mesh& mesh;
    const int* indices;
};

std::vector<std::shared_ptr<Shape>> createTriangleMesh(const Mesh& mesh) {
    std::vector<std::shared_ptr<Shape>> trianles;
    auto nTriangles = mesh.indices.size() / 3;
    for (auto i = 0; i < nTriangles; ++i) {
        trianles.push_back(std::make_shared<Triangle>(mesh, i));
    }
    return trianles;
}

}

#endif
