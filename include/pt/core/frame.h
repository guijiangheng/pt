#ifndef PT_CORE_FRAME_H
#define PT_CORE_FRAME_H

#include <pt/math/matrix4.h>
#include <pt/core/interaction.h>

namespace pt {

class Frame {
public:
    Frame() : m(Matrix4::identity()), mInv(Matrix4::identity())
    { }

    Frame(const Matrix4& m) : m(m), mInv(inverse(m))
    { }

    Frame (const Matrix4& m, const Matrix4& mInv) : m(m), mInv(mInv)
    { }

    Vector3 toLocalV(const Vector3& v) const {
        return mInv.applyV(v);
    }

    Vector3 toGlobalV(const Vector3& v) const {
        return m.applyV(v);
    }

    Vector3 toLocalN(const Vector3& n) const {
        return mInv.applyN(n);
    }

    Vector3 toGlobalN(const Vector3& n) const {
        return m.applyN(n);
    }

    Vector3 toLocalP(const Vector3& p) const {
        return mInv.applyP(p);
    }

    Vector3 toGlobalP(const Vector3& p) const {
        return m.applyP(p);
    }

    Ray toLocal(const Ray& r) const {
        return Ray(toLocalP(r.o), toLocalV(r.d), r.tMax);
    }

    Ray toGlobal(const Ray& r) const {
        return Ray(toGlobalP(r.o), toGlobalV(r.d), r.tMax);
    }

    Interaction toLocal(const Interaction& isect) const {
        return Interaction(
            toLocalP(isect.p),
            toLocalN(isect.n),
            toLocalV(isect.wo)
        );
    }

    Interaction toGlobal(const Interaction& isect) const {
        return Interaction(
            toGlobalP(isect.p),
            toGlobalN(isect.n),
            toGlobalV(isect.wo)
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