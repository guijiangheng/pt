#include <array>
#include <algorithm>
#include <pt/accelerators/bvh.h>

namespace pt {

struct PrimInfo {
    PrimInfo() = default;

    PrimInfo(int primIndex, const Bounds3& bounds) noexcept
        : primIndex(primIndex)
        , bounds(bounds)
        , center((bounds.pMin + bounds.pMax) * (Float)0.5)
    { }

    int primIndex;
    Bounds3 bounds;
    Vector3 center;
};

struct BVHNode {
    BVHNode() = default;

    BVHNode(const Bounds3& b, int primsOffset, int nPrims) noexcept
        : bounds(b)
        , primsOffset(primsOffset)
        , nPrims(nPrims)
        , left(nullptr), right(nullptr)
    { }

    BVHNode(const Bounds3& bounds, int axis, BVHNode* left, BVHNode* right) noexcept
        : bounds(bounds)
        , left(left) , right(right)
        , splitAxis(axis), primsOffset(0), nPrims(0)
    { }

    Bounds3 bounds;
    BVHNode *left, *right;
    int splitAxis, primsOffset, nPrims;
};

struct Bucket {
    int count = 0;
    Bounds3 bounds;
};

BVHAccel::BVHAccel(std::vector<Primitive*>&& prims) noexcept : primitives(std::move(prims)) {
    auto size = primitives.size();
    std::vector<PrimInfo> primInfos;
    primInfos.reserve(size);
    for (auto i = 0; i < size; ++i)
        primInfos.emplace_back(i, primitives[i]->getBounds());

    int totalNodes = 0;
    std::vector<Primitive*> orderedPrims;
    orderedPrims.reserve(size);
    auto root = sahBuild(primInfos, 0, size, totalNodes, orderedPrims);
    primitives = std::move(orderedPrims);

    nodes.reserve(totalNodes);
    flattenBVHTree(root);
    destroyBVHTree(root);
}

BVHNode* BVHAccel::createLeafNode(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Primitive*>& orderedPrims) const {

    ++totalNodes;
    Bounds3 bounds;
    for (auto i = start; i < end; ++i) {
        bounds.expandBy(primInfos[i].bounds);
        orderedPrims.push_back(primitives[primInfos[i].primIndex]);
    }
    return new BVHNode(bounds, start, end - start);
}

BVHNode* BVHAccel::exhaustBuild(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Primitive*>& orderedPrims) const {
    
    auto nPrims = end - start;
    if (nPrims == 1)
        return createLeafNode(primInfos, start, end, totalNodes, orderedPrims);

    Float totalAreaInv;
    Bounds3 totalBounds;
    auto totalBoundsInitialized = false;

    int splitPrim;
    int splitAxis = -1;
    Float minCost = nPrims;

    for (int axis = 0; axis < 3; ++axis) {
        std::sort(&primInfos[start], &primInfos[end - 1] + 1, [=](auto& a, auto& b) {
            return a.center[axis] < b.center[axis];
        });

        Bounds3 rightBounds[SAH_APPLY_COUNT];
        for (auto i = nPrims - 2; i >= 0; --i)
            rightBounds[i] = merge(rightBounds[i + 1], primInfos[start + i + 1].bounds);

        if (!totalBoundsInitialized) {
            totalBoundsInitialized = true;
            totalBounds = merge(primInfos[start].bounds, rightBounds[0]);
            totalAreaInv = 1 / totalBounds.area();
        }

        Bounds3 leftBound;
        for (auto i = 0; i < nPrims - 1; ++i) {
            auto counts = i + 1;
            leftBound.expandBy(primInfos[start + i].bounds);
            Float cost = AABB_SHAPE_INTERSECT_COST_RATIO +
                (counts * leftBound.area() + (nPrims - counts) * rightBounds[i].area()) * totalAreaInv;
            if (cost < minCost) {
                splitAxis = axis;
                splitPrim = i;
                minCost = cost;
            }
        }
    }

    if (splitAxis == -1)
        return createLeafNode(primInfos, start, end, totalNodes, orderedPrims);

    std::sort(&primInfos[start], &primInfos[end - 1] + 1, [=](auto& a, auto& b) {
        return a.center[splitAxis] < b.center[splitAxis];
    });

    ++totalNodes;
    return new BVHNode(
        totalBounds, splitAxis,
        exhaustBuild(primInfos, start, start + splitPrim + 1, totalNodes, orderedPrims),
        exhaustBuild(primInfos, start + splitPrim + 1, end, totalNodes, orderedPrims)
    );
}

BVHNode* BVHAccel::sahBuild(
    std::vector<PrimInfo>& primInfos,
    int start, int end, int& totalNodes,
    std::vector<Primitive*>& orderedPrims) const {

    auto nPrims = end - start;
    if (nPrims == 1) 
        return createLeafNode(primInfos, start, end, totalNodes, orderedPrims);

    if (nPrims < SAH_APPLY_COUNT)
        return exhaustBuild(primInfos, start, end, totalNodes, orderedPrims);

    Bounds3 centerBounds;
    for (auto i = start; i < end; ++i)
        centerBounds.expandBy(primInfos[i].center);
    int dim = centerBounds.maxExtent();

    if (centerBounds.pMax[dim] - centerBounds.pMin[dim] < (Float)(0.00001))
        return createLeafNode(primInfos, start, end, totalNodes, orderedPrims);

    Bucket buckets[BUCKETS];
    auto dist = centerBounds.pMax[dim] - centerBounds.pMin[dim];

    for (auto i = start; i < end; ++i) {
        auto offset = primInfos[i].center[dim] - centerBounds.pMin[dim];
        auto b = (int)(BUCKETS * offset / dist);
        if (b == BUCKETS) b = BUCKETS - 1;
        ++buckets[b].count;
        buckets[b].bounds.expandBy(primInfos[i].bounds);
    }

    Bounds3 rightBounds[BUCKETS];
    for (auto i = BUCKETS - 2; i >= 0; --i)
        rightBounds[i] = merge(rightBounds[i + 1], buckets[i + 1].bounds);
    Bounds3 totalBounds = merge(rightBounds[0], buckets[0].bounds);
    auto totalAreaInv = 1 / totalBounds.area();
    
    int splitBucket = -1;
    int counts = 0;
    Float minCost = nPrims;
    Bounds3 leftBound;

    for (auto i = 0; i < BUCKETS - 1; ++i) {
        counts += buckets[i].count;
        leftBound.expandBy(buckets[i].bounds);
        Float cost = AABB_SHAPE_INTERSECT_COST_RATIO +
            (counts * leftBound.area() + (nPrims - counts) * rightBounds[i].area()) * totalAreaInv;
        if (cost < minCost) {
            splitBucket = i;
            minCost = cost;
        }
    }

    if (splitBucket == -1)
        return exhaustBuild(primInfos, start, end, totalNodes, orderedPrims);

    auto pmid = std::partition(&primInfos[start], &primInfos[end - 1] + 1, [=](auto& p) {
        auto offset = p.center[dim] - centerBounds.pMin[dim];
        auto b = (int)(BUCKETS * offset / dist);
        if (b == BUCKETS)  b = BUCKETS - 1;
        return b <= splitBucket;
    });
    auto mid = pmid - &primInfos[0];

    ++totalNodes;
    return new BVHNode(
        totalBounds, dim,
        sahBuild(primInfos, start, mid, totalNodes, orderedPrims),
        sahBuild(primInfos, mid, end, totalNodes, orderedPrims)
    );
}

void BVHAccel::destroyBVHTree(const BVHNode* node) const {
    if (node == nullptr) return;
    if (node->nPrims == 0) {
        destroyBVHTree(node->left);
        destroyBVHTree(node->right);
    }
    delete node;
}

void BVHAccel::flattenBVHTree(const BVHNode* node) {
    if (node->nPrims) {
        nodes.emplace_back(node->bounds, node->primsOffset, (uint16_t)node->nPrims);
    } else {
        nodes.emplace_back(node->bounds, node->splitAxis);
        auto& linearNode = nodes.back();
        flattenBVHTree(node->left);
        linearNode.rightChild = (int)nodes.size();
        flattenBVHTree(node->right);
    }
}

bool BVHAccel::intersect(const Ray& ray, Interaction& isect) const {
    Vector3 invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
    const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

    auto hit = false;
    int nodesToVisit[64];
    nodesToVisit[0] = 0;
    int currentIndex, toVisitOffset = 0;
    
    while (toVisitOffset != -1) {
        currentIndex = nodesToVisit[toVisitOffset--];
        auto& node = nodes[currentIndex];
        if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
            if (node.nPrims) {
                for (auto i = 0; i < node.nPrims; ++i)
                    if (primitives[node.primsOffset + i]->intersect(ray, isect))
                        hit = true;
            } else {
                if (dirIsNeg[node.splitAxis]) {
                    nodesToVisit[++toVisitOffset] = currentIndex + 1;
                    nodesToVisit[++toVisitOffset] = node.rightChild;
                } else {
                    nodesToVisit[++toVisitOffset] = node.rightChild;
                    nodesToVisit[++toVisitOffset] = currentIndex + 1;
                }
            }
        }
    }

    return hit;
}

bool BVHAccel::intersect(const Ray& ray) const {
    Vector3 invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
    const int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };

    int nodesToVisit[64];
    nodesToVisit[0] = 0;
    int currentIndex, toVisitOffset = 0;
    
    while (toVisitOffset != -1) {
        currentIndex = nodesToVisit[toVisitOffset--];
        auto& node = nodes[currentIndex];
        if (node.bounds.intersect(ray, invDir, dirIsNeg)) {
            if (node.nPrims) {
                for (auto i = 0; i < node.nPrims; ++i)
                    if (primitives[node.primsOffset + i]->intersect(ray))
                        return true;                        
            } else {
                if (dirIsNeg[node.splitAxis]) {
                    nodesToVisit[++toVisitOffset] = currentIndex + 1;
                    nodesToVisit[++toVisitOffset] = node.rightChild;
                } else {
                    nodesToVisit[++toVisitOffset] = node.rightChild;
                    nodesToVisit[++toVisitOffset] = currentIndex + 1;
                }
            }
        }
    }

    return false;
}

}