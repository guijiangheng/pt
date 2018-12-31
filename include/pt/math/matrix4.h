#ifndef PT_MATH_MATRIX4_H
#define PT_MATH_MATRIX4_H

#include <cstring>
#include <pt/core/ray.h>

namespace pt {

class Matrix4 {
public:
    Matrix4() = default;

    Matrix4(const Float m[4][4]) noexcept {
        std::memcpy(e, m, 16 * sizeof(Float));
    }

    Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) noexcept : e {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    } { }

    Matrix4 operator*(const Matrix4& m) const {
        Float r[4][4];
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                r[i][j] = e[i][0] * m.e[0][j] + e[i][1] * m.e[1][j] +
                            e[i][2] * m.e[2][j] + e[i][3] * m.e[3][j];
        return Matrix4(r);
    }

    Matrix4& operator*=(const Matrix4& m) {
        *this *= m;
        return *this;
    }

    Vector3 applyV(const Vector3& v) const {
        return Vector3(
            e[0][0] * v.x + e[0][1] * v.y + e[0][2] * v.z,
            e[1][0] * v.x + e[1][1] * v.y + e[1][2] * v.z,
            e[2][0] * v.x + e[2][1] * v.y + e[2][2] * v.z
        );
    }

    Vector3 applyN(const Vector3& n) const {
        return Vector3(
            e[0][0] * n.x + e[1][0] * n.y + e[2][0] * n.z,
            e[0][1] * n.x + e[1][1] * n.y + e[2][1] * n.z,
            e[0][2] * n.x + e[1][2] * n.y + e[2][2] * n.z
        );
    }

    Vector3 applyP(const Vector3& p) const {
        auto x = e[0][0] * p.x + e[0][1] * p.y + e[0][2] * p.z + e[0][3];
        auto y = e[1][0] * p.x + e[1][1] * p.y + e[1][2] * p.z + e[1][3];
        auto z = e[2][0] * p.x + e[2][1] * p.y + e[2][2] * p.z + e[2][3];
        auto w = e[3][0] * p.x + e[3][1] * p.y + e[3][2] * p.z + e[3][3];
        if (w == 1)
            return Vector3(x, y, z);
        return Vector3(x, y, z) / w;
    }

    Ray operator()(const Ray& r) const {
        return Ray(applyP(r.o), applyV(r.d), r.tMax);
    }

    Matrix4& inverse();

    Matrix4& transpose();

    bool operator==(const Matrix4& m) const {
        for (auto r = 0; r < 4; ++r)
            for (auto c = 0; c < 4; ++c)
                if (e[r][c] != m.e[r][c]) return false;
        return true;
    }

    bool operator!=(const Matrix4& m) const {
        for (auto r = 0; r < 4; ++r)
            for (auto c = 0; c < 4; ++c)
                if (e[r][c] != m.e[r][c]) return true;
        return false;
    }

    static Matrix4 identity() {
        return Matrix4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }

    static Matrix4 translate(Float x, Float y, Float z) {
        return Matrix4(
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        );
    }

    static Matrix4 scale(Float x, Float y, Float z) {
        return Matrix4(
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        );
    }

    static Matrix4 rotateX(Float angle) {
        auto s = std::sin(radians(angle));
        auto c = std::cos(radians(angle));
        return Matrix4(
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        );
    }

    static Matrix4 rotateY(Float angle) {
        auto s = std::sin(radians(angle));
        auto c = std::cos(radians(angle));
        return Matrix4(
             c, 0, s, 0,
             0, 1, 0, 0,
            -s, 0, c, 0,
             0, 0, 0, 1 
        );
    }

    static Matrix4 rotateZ(Float angle) {
        auto s = std::sin(radians(angle));
        auto c = std::cos(radians(angle));
        return Matrix4(
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        );
    }

    static Matrix4 rotate(const Vector3& axis, Float angle) {
        auto s = std::sin(radians(angle));
        auto c = std::cos(radians(angle));
        auto x = axis.x, y = axis.y, z = axis.z;
        auto xx = x * x, xy = x * y, xz = x * z, yy = y * y, yz = y * z, zz = z * z;
        return Matrix4(
            xx + (1 - xx) * c, xy * (1 - c) - z * s, xz * (1 - c) + y * s, 0,
            xy * (1 - c) + z * s, yy + ( 1- yy) * c, yz * (1 - c) - x * s, 0,
            xz * (1 - c) - y * s, yz * (1 - c) + x * s, zz + (1 - zz) * c, 0,
            0, 0, 0, 1
        );
    }

    static Matrix4 lookAt(const Vector3& pos, const Vector3& target, const Vector3& up) {
        auto z = normalize(target - pos);
        auto x = normalize(cross(up, z));
        auto y = cross(z, x);
        return Matrix4(
            x.x, y.x, z.x, pos.x,
            x.y, y.y, z.y, pos.y,
            x.z, y.z, z.z, pos.z,
            0, 0, 0, 1
        );
    }

    static Matrix4 perspective(Float fov, Float near, Float far) {
        Float d = 1 / std::tan(radians(fov / 2));
        return Matrix4(
            d, 0, 0, 0,
            0, d, 0, 0,
            0, 0, far / (far - near), near * far / (near - far),
            0, 0, 1, 0
        );
    }

public:
    Float e[4][4];
};

inline Matrix4 inverse(const Matrix4& m) {
    Matrix4 r = m;
    return r.inverse();
}

inline Matrix4 transpose(const Matrix4& m) {
    return Matrix4(
        m.e[0][0], m.e[1][0], m.e[2][0], m.e[3][0],
        m.e[0][1], m.e[1][1], m.e[2][1], m.e[3][1],
        m.e[0][2], m.e[1][2], m.e[2][2], m.e[3][2],
        m.e[0][3], m.e[1][3], m.e[2][3], m.e[3][3]
    );
}

}

#endif