#pragma once
#include "geometry/primitive.h"


// Forward declarations of structs used by our BVH tree
// They are defined in the .cpp file
struct BVHBuildNode;
struct BVHPrimitiveInfo;
struct MortonPrimitive;
struct LinearBVHNode;

class BVHAccel : public Primitive
{
    //Functions
public:

    BVHAccel(const std::vector<std::shared_ptr<Primitive>> &p,
             int maxPrimsInNode = 1);
    Bounds3f WorldBound() const;
    ~BVHAccel();
    bool Intersect(const Ray &ray, Intersection *isect) const;

private:
    BVHBuildNode *recursiveBuild(
        std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int start, int end, int *totalNodes,
        std::vector<std::shared_ptr<Primitive>> &orderedPrims);

    BVHBuildNode *buildUpperSAH(std::vector<BVHBuildNode *> &treeletRoots,
                                int start, int end, int *totalNodes) const;

    int flattenBVHTree(BVHBuildNode *node, int *offset);


    //Members
    const int maxPrimsInNode;
    std::vector<std::shared_ptr<Primitive>> primitives;
    LinearBVHNode *nodes = nullptr;



    BVHBuildNode *root = nullptr;   //I MADE THIS TO SAVE THE ROOT OF THE TREE IN BVH::ACCEL
};
