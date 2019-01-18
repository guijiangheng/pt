#ifndef PT_UTILS_OBJLOADER_H
#define PT_UTILS_OBJLOADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <pt/shapes/triangle.h>

namespace pt {

std::vector<std::string> tokenize(const std::string& string, const std::string& delim) {
    std::vector<std::string> tokens;
    std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);

    while (lastPos != std::string::npos) {
        tokens.push_back(string.substr(lastPos, pos - lastPos));
        lastPos = pos;
        if (lastPos != std::string::npos) {
            lastPos += 1;
            pos = string.find_first_of(delim, lastPos);
        }
    }

    return tokens;
}

class ObjVertex {
public:
    ObjVertex() noexcept : p(0), n(0), uv(0)
    { }

    ObjVertex(const std::string& string) {
        auto tokens = tokenize(string, "/");

        if (tokens.size() > 3)
            throw std::runtime_error("Invalid vertex data!");

        p = std::atoi(tokens[0].c_str());

        if (tokens.size() >= 2 && !tokens[1].empty())
            uv = std::atoi(tokens[1].c_str());
        
        if (tokens.size() == 3 && !tokens[2].empty())
            n = std::atoi(tokens[2].c_str());
    }

    bool operator==(const ObjVertex& v) const {
        return p == v.p && n == v.n && uv == v.uv;
    }

    bool operator!=(const ObjVertex& v) const {
        return p != v.p || n != v.n || uv != v.uv;
    }

public:
    int p, n, uv;
};

Mesh loadObjMesh(const std::string& filename) {
    std::ifstream stream(filename);

    if (stream.fail()) {
        throw std::runtime_error("Unable to open OBJ file!");
    }

    std::cout << "Loading \"" << filename << "\" ... " << std::endl;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2f> uvs;
    std::vector<int> indices;
    std::vector<ObjVertex> vertices;

    auto hash = [](const ObjVertex& v) {
        auto hash = std::hash<int>()(v.p);
        hash = hash * 37 + std::hash<int>()(v.uv);
        hash = hash * 37 + std::hash<int>()(v.n);
        return hash;
    };

    std::string lineStr;
    std::unordered_map<ObjVertex, int, decltype(hash)> map(200, hash);

    while (std::getline(stream, lineStr)) {
        std::istringstream line(lineStr);
        std::string prefix;
        line >> prefix;

        if (prefix == "v") {
            Vector3 p;
            line >> p.x >> p.y >> p.z;
            p.z = -p.z;
            positions.push_back(p);
        } else if (prefix == "vt") {
            Vector2f uv;
            line >> uv.x >> uv.y;
            uvs.push_back(uv);
        } else if (prefix == "vn") {
            Vector3 n;
            line >> n.x >> n.y >> n.z;
            n.z = -n.z;
            normals.push_back(n);
        } else if (prefix == "f") {
            std::string v1, v2, v3, v4;
            line >> v1 >> v2 >> v3 >> v4;
            ObjVertex verts[6];
            int nVertices = 3;

            verts[0] = ObjVertex(v1);
            verts[1] = ObjVertex(v2);
            verts[2] = ObjVertex(v3);

            if (!v4.empty()) {
                verts[3] = ObjVertex(v4);
                verts[4] = verts[0];
                verts[5] = verts[2];
                nVertices = 6;
            }

            for (auto i = 0; i < nVertices; ++i) {
                auto& v = verts[i];
                auto it = map.find(v);
                if (it == map.end()) {
                    map[v] = vertices.size();
                    indices.push_back(vertices.size());
                    vertices.push_back(v);
                } else {
                    indices.push_back(it->second);
                }
            }
        }
    }

    std::vector<Vector3> meshVertices;
    meshVertices.reserve(vertices.size());
    for (auto& v : vertices) {
        meshVertices.push_back(positions[v.p - 1]);
    }

    std::vector<Vector3> meshNormals;
    if (!normals.empty()) {
        meshNormals.reserve(vertices.size());
        for (auto& v : vertices) {
            meshNormals.push_back(normals[v.n - 1]);
        }
    }

    std::vector<Vector2f> meshUvs;
    if (!uvs.empty()) {
        meshUvs.reserve(vertices.size());
        for (auto& v : vertices) {
            meshUvs.push_back(uvs[v.uv - 1]);
        }
    }

    std::cout << "Done. V=" << meshVertices.size()
              << ", F=" << indices.size() / 3 << std::endl;

    return Mesh(
        std::move(indices),
        std::move(meshVertices),
        std::move(meshNormals),
        std::move(meshUvs)
    );
}

}

#endif
