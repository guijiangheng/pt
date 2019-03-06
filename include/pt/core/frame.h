#ifndef PT_CORE_FRAME_H
#define PT_CORE_FRAME_H

#include <pt/math/bounds3.h>
#include <pt/math/matrix4.h>
#include <pt/core/interaction.h>

namespace pt {

class Frame {
public:
    Frame() noexcept : m(Matrix4::identity()), mInv(Matrix4::identity())
    { }

    Frame(const Matrix4& m) noexcept : m(m), mInv(inverse(m))
    { }

    Frame (const Matrix4& m, const Matrix4& mInv) noexcept : m(m), mInv(mInv)
    { }

    Frame operator*(const Frame& frame) const {
        return Frame(m * frame.m, frame.mInv * mInv);
    }

    Vector3 toLocalV(const Vector3& v) const {
        return mInv.applyV(v);
    }

    Vector3 toWorldV(const Vector3& v) const {
        return m.applyV(v);
    }

    Vector3 toLocalN(const Vector3& n) const {
        return mInv.applyN(n);
    }

    Vector3 toWorldN(const Vector3& n) const {
        return m.applyN(n);
    }

    Vector3 toLocalP(const Vector3& p) const {
        return mInv.applyP(p);
    }

    Vector3 toWorldP(const Vector3& p) const {
        return m.applyP(p);
    }

    Ray toLocal(const Ray& r) const {
        return Ray(toLocalP(r.o), toLocalV(r.d), r.tMax);
    }

    Ray toWorld(const Ray& r) const {
        return Ray(toWorldP(r.o), toWorldV(r.d), r.tMax);
    }

    Bounds3 toWorld(const Bounds3& b) const {
        Bounds3 ret(toWorldP(b.pMin));
        ret.expandBy(toWorldP(Vector3(b.pMin.x, b.pMin.y, b.pMax.z)));
        ret.expandBy(toWorldP(Vector3(b.pMin.x, b.pMax.y, b.pMin.z)));
        ret.expandBy(toWorldP(Vector3(b.pMin.x, b.pMax.y, b.pMax.z)));
        ret.expandBy(toWorldP(Vector3(b.pMax.x, b.pMin.y, b.pMin.z)));
        ret.expandBy(toWorldP(Vector3(b.pMax.x, b.pMin.y, b.pMax.z)));
        ret.expandBy(toWorldP(Vector3(b.pMax.x, b.pMax.y, b.pMin.z)));
        ret.expandBy(toWorldP(Vector3(b.pMax.x, b.pMax.y, b.pMax.z)));
        return ret;
    }

    Interaction toLocal(const Interaction& isect) const {
        return Interaction(
            toLocalP(isect.p),
            toLocalN(isect.n),
            toLocalV(isect.wo)
        );
    }

    Interaction toWorld(const Interaction& isect) const {
        return Interaction(
            toWorldP(isect.p),
            toWorldN(isect.n),
            toWorldV(isect.wo)
        );
    }

    static Frame translate(Float x, Float y, Float z) {
        return Frame(
            Matrix4::translate(x, y, z),
            Matrix4::translate(-x, -y, -z)
        );
    }

    static Frame scale(Float x, Float y, Float z) {
        return Frame(
            Matrix4::scale(x, y, z),
            Matrix4::scale(1 / x, 1 / y, 1 / z)
        );
    }

    static Frame rotateX(Float angle) {
        auto m = Matrix4::rotateX(angle);
        return Frame(m, transpose(m));
    }

    static Frame rotateY(Float angle) {
        auto m = Matrix4::rotateY(angle);
        return Frame(m, transpose(m));
    }

    static Frame rotateZ(Float angle) {
        auto m = Matrix4::rotateZ(angle);
        return Frame(m, transpose(m));
    }

    static Frame rotate(const Vector3& aixs, Float angle) {
        auto m = Matrix4::rotate(aixs, angle);
        return Frame(m, transpose(m));
    }

    static Frame lookAt(const Vector3& pos, const Vector3& target, const Vector3& up) {
        auto m = Matrix4::lookAt(pos, target, up);
        return Frame(m, transpose(m));
    }

private:
    Matrix4 m, mInv;
};

}

#endif
