#include <random>
#include <vector>
#include <fstream>
#include <iostream>

#include <pt/core/ray.h>
#include <pt/shapes/sphere.h>
#include <pt/core/primitive.h>

using namespace pt;
using namespace std;

constexpr auto EPSILON = (Float)0.00001;

random_device dev;
mt19937 gen;
uniform_real_distribution<Float> d;

Vector3 randomUnitSphere() {
    Vector3 p;
    do {
        p = Vector3(d(gen), d(gen), d(gen)) * 2 - Vector3(1, 1, 1);
    } while (p.lengthSquared() > 1);
    return p;
}

Vector3 color(const vector<GeometricPrimitive>& scene, const Ray& r) {
    bool hit = false;
    Interaction isect;

    for (auto obj : scene) {
        if (obj.intersect(r, isect))
            hit = true;
    }

    if (hit) {
        Vector3 o = isect.p + isect.n * EPSILON;
        Vector3 target = isect.p + isect.n + randomUnitSphere();
        return color(scene, Ray(o, normalize(target - o))) * 0.5;
    }

    auto t = (r.d.y() + 1) / 2;
    return lerp(Vector3(0.5, 0.7, 1.0), Vector3(1, 1, 1), t);
}



int main() {
    constexpr auto width = 400;
    constexpr auto height = 200;
    constexpr auto samples = 100;
    
    vector<Vector3> pixels(width * height);

    GeometricPrimitive a(Frame(), std::make_shared<Sphere>(0.5));
    GeometricPrimitive b(Frame::translate(0, -100.5, 0), std::make_shared<Sphere>(100));

    vector<GeometricPrimitive> scene;
    scene.push_back(a);
    scene.push_back(b);

    fstream file("./image.pfm", ios::out | ios::binary);
    file << "PF\n" << width << " " << height << "\n" << "-1\n";

    Vector3 lowerLeftCorner(-2, -1, 0);
    Vector3 horizotal(4, 0, 0);
    Vector3 vertical(0, 2, 0);

    random_device dev;
    mt19937 gen(dev());
    std::uniform_real_distribution<Float> d(0, 1);

    Vector3 c(0, 0, 0);
    for (auto y = height - 1; y >= 0; --y)
        for (auto x = 0; x < width; ++x) {
            for (auto s = 0; s < samples; ++s) {
                auto u = (x + d(gen)) / (Float)width;
                auto v = 1 - (y + d(gen)) / (Float)height;
                Vector3 eye(0, 0, -1);
                auto target = lowerLeftCorner + horizotal * u + vertical * v;
                c += color(scene, Ray(eye, normalize(target - eye)));
            }
            c /= samples;
            file.write((char*)&c, sizeof(Vector3));
        }

	file.close();

    return 0;
}