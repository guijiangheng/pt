#ifndef PT_ACCELATORS_BVH_H
#define PT_ACCELATORS_BVH_H

#include <vector>
#include <pt/math/bounds3.h>
#include <pt/core/primitive.h>

namespace pt {

class BVHNode;
class PrimInfo;

struct LinearBVHNode {
    Bounds3 bounds;
    uint16_t nPrims;
    uint16_t splitAxis;
    union {
        int primsOffset;
        int rightChild;
    };

    // leaf node constructor
    LinearBVHNode(const Bounds3& bounds, int primsOffset, uint16_t nPrims)
        : bounds(bounds), nPrims(nPrims), primsOffset(primsOffset)
    { }

    // interior node constructor
    LinearBVHNode(const Bounds3& bounds, uint16_t splitAxis)
        : bounds(bounds), nPrims(0), splitAxis(splitAxis)
    { }
};

class BVHAccel : public Primitive {
public:
    BVHAccel(std::vector<Primitive*>&& prims) noexcept;

    ~BVHAccel() noexcept {
        for (auto p : primitives) {
            delete p;
        }
    }

    Bounds3 worldBound() const override {
        return nodes[0].bounds;
    }

    bool intersect(const Ray& ray, Interaction& isect) const override;

    bool intersect(const Ray& ray) const override;

    const Material* getMaterial() const override {
        throw std::runtime_error("Only ShapePrimitive supports getMaterial methods!");
    };

    const DiffuseAreaLight* getAreaLight() const override {
        throw std::runtime_error("Only ShapePrimitive supports getAreaLight methods!");
    }

    void computeScatteringFunctions(Interaction& isect) const override {
        throw std::runtime_error("Only ShapePrimitive supports computeScatteringFunctions methods!");
    }

private:
    BVHNode* createLeafNode(
        std::vector<PrimInfo>& primInfos,
        int start, int end, int& totalNodes,
        std::vector<Primitive*>& orderedPrims) const;

    BVHNode* exhaustBuild(
        std::vector<PrimInfo>& primInfos,
        int start, int end, int& totalNodes,
        std::vector<Primitive*>& orderedPrims) const;

    BVHNode* sahBuild(
        std::vector<PrimInfo>& primInfos,
        int start, int end, int& totalNodes,
        std::vector<Primitive*>& orderedPrims) const;

    void destroyBVHTree(const BVHNode* node) const;

    void flattenBVHTree(const BVHNode* node);


public:
    static constexpr int BUCKETS = 16;
    static constexpr int SAH_APPLY_COUNT = 32;
    static constexpr Float AABB_SHAPE_INTERSECT_COST_RATIO = (Float)1 / 4;

private:
    std::vector<Primitive*> primitives;
    std::vector<LinearBVHNode> nodes;
};

}

#endif
