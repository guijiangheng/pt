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
        : bounds(bounds), primsOffset(primsOffset), nPrims(nPrims)
    { }

    // interior node constructor
    LinearBVHNode(const Bounds3& bounds, uint16_t splitAxis)
        : bounds(bounds), splitAxis(splitAxis), nPrims(0)
    { }
};

class BVHAccel : public Primitive {
public:
    BVHAccel(std::vector<Primitive*>&& prims) noexcept;

    ~BVHAccel() {
        for (auto p : primitives) {
            delete p;
        }
    }

    Bounds3 getBounds() const override {
        return nodes[0].bounds;
    }

    bool intersect(const Ray& ray, Interaction& isect) const override;

    bool intersect(const Ray& ray) const override;

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