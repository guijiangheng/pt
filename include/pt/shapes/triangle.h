#ifndef PT_SHAPES_TRIANGLE_H
#define PT_SHAPES_TRIANGLE_H

#include <vector>
#include <pt/pt.h>
#include <pt/core/shape.h>
#include <pt/core/sampling.h>

namespace pt {

class Mesh {
public:
    Mesh(std::vector<int>&& indices,
         std::vector<Vector3>&& vertices,
         std::vector<Vector3>&& normals = std::vector<Vector3>(),
         std::vector<Vector2f>&& uvs = std::vector<Vector2f>()) noexcept
            : indices(std::move(indices))
            , vertices(std::move(vertices))
            , normals(std::move(normals))
            , uvs(std::move(uvs))
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
        : Shape(nullptr), mesh(mesh), indices(&mesh.indices[triangleIndex * 3])
    { }

    Bounds3 objectBound() const override {
        return worldBound();
    }

    Bounds3 worldBound() const override {
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
        isect.wo = -ray.d;
        
        if (mesh.normals.size() > 0) {
            auto& na = mesh.normals[indices[0]];
            auto& nb = mesh.normals[indices[1]];
            auto& nc = mesh.normals[indices[2]];
            isect.n = normalize(na * (1 - u - v) + nb * u + nc * v);
        } else {
            isect.n = normalize(cross(edge2, edge1));
        }

        return true;
    }

    Float area() const override {
        auto& a = mesh.vertices[indices[0]];
        auto& b = mesh.vertices[indices[1]];
        auto& c = mesh.vertices[indices[2]];
        return cross(b - a, c - a).length() / 2;
    }

    Interaction sample(const Vector2f& u, Float& pdf) const override {
        auto uv = uniformSampleTriangle(u);
        auto& a = mesh.vertices[indices[0]];
        auto& b = mesh.vertices[indices[1]];
        auto& c = mesh.vertices[indices[2]];
        Interaction isect;
        isect.p = a * uv[0] + b * uv[1] + c * (1 - uv[0] - uv[1]);
        isect.n = normalize(cross(c - a, b - a));
        pdf = 1 / area();
        return isect;
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
