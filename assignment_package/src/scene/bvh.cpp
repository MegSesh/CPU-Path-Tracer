#include "bvh.h"

// Feel free to ignore these structs entirely!
// They are here if you want to implement any of PBRT's
// methods for BVH construction.

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
};

struct BVHBuildNode {
    // BVHBuildNode Public Methods
    void InitLeaf(int first, int n, const Bounds3f &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;        //leaf nodes don't have children
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;                           //children nodes need to have already been created so that pointers can be passed in
        children[1] = c1;
        bounds = Union(c0->bounds, c1->bounds);     //bounds of all children beneath the node
        splitAxis = axis;                           //axis --> axis along which primitives were split for distribution for their 2 children (used for traversal performance improvement)
        nPrimitives = 0;                            //primitives are only stored in leaf nodes
    }
    Bounds3f bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct MortonPrimitive {
    int primitiveIndex;
    unsigned int mortonCode;
};

struct LBVHTreelet {
    int startIndex, nPrimitives;
    BVHBuildNode *buildNodes;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};




BVHAccel::~BVHAccel()
{
    delete [] nodes;
}



/* ==================================== BVHACCEL CLASS ====================================
 * We have provided you with a class that represents a bounding volume hierarchy, BVHAccel.

 * Most of the details of implementation are left for you to decide, but you will need to implement
 * at least two of the functions we have provided:

                        BVHAccel's constructor and its Intersect function.


 * The constructor is where you should build the hierarchy of bounding box nodes;
 * the only detail we require of this construction is that you use the Surface Area Heuristic
 * to determine where to split your geometry. We have provided you with the intermediate structs
 * that PBRT uses in its implementation of its BVH, but we do not require you to use any of them.

 * A branch of BVH construction should produce a leaf node when there would be at most
 * maxPrimsInNode Primitives within that node, or when the cost of subdividing that node
 * via the surface area heuristic would be greater than the cost of simply testing for intersection
 * against all primitives in that node.


 * Remember that the SAH cost of a subdivision is estimated as

                (surface area of left box * number of primitives in left box +
                        surface area of right box * number of primitives in right box )
                / (surface area of encompassing box).


 * Likewise the cost of intersecting a node is simply number of primitives in the node.


 RANDOM NOTES

 * &p --> the primitives to be stored
 * maxPrimsInNode --> maximum number of primitives that can be in any leaf node
 * (in book) SplitMethod --> enumerant value that describes which algorithm to use
            to split primitives to build the tree. We'll be using Surface Area Heuristic (Section 4.3.2)


       3 steps to BVH construction

           1. Bounding info of each primitive is computed and stored in an array to use during tree construction

           2. Tree is built using Surface Area Heuristic. Result is a binary tree where each interior node
                    holds pointers to its children and each leaf node holds references to 1+ primitives

           3. Tree is converted to more compact pointerless representation for use during rendering

RECURSIVE BUILD --> 259
SAH --> PAGE 263
 */



// Constructs an array of BVHPrimitiveInfos, recursively builds a node-based BVH
// from the information, then optimizes the memory of the BVH
BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive> > &p, int maxPrimsInNode)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
    //TODO

    if(primitives.size() == 0)
    {
        return;
    }

    //initialize primitiveInfo array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfoList(primitives.size());
    for(size_t i = 0; i < primitives.size(); i++)
    {
        primitiveInfoList[i] = {    i , primitives[i].get()->shape->WorldBound()    };
    }//end for loop


    //build tree using using SAH with primitiveInfoList
    int totalNumNodes = 0;
    std::vector<std::shared_ptr<Primitive>> orderedPrimitives;
    root = recursiveBuild(primitiveInfoList, 0, primitives.size(), &totalNumNodes, orderedPrimitives);
    primitives.swap(orderedPrimitives);

    nodes = (LinearBVHNode*)malloc(totalNumNodes * sizeof *nodes);
    int offset = 0;
    flattenBVHTree(root, &offset);
}//end Accel function


//PAGE 282 IN BOOK
//do a breadth first search for overlapping primitives. depth first for non overlapping ones
bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    //TODO

    bool hit = false;

    Vector3f invDir(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);

    int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

    // Follow ray through BVH nodes to find primitive intersections
    int toVisitOffset = 0;
    int currentNodeIndex = 0;
    int nodesToVisit[64];

    float minimumT = 10000000.0f;
    Intersection minimumIsect = Intersection();

    while (true)
    {
        const LinearBVHNode *node = &nodes[currentNodeIndex];

        // Check ray against BVH node
        if (node->bounds.IntersectP(ray, invDir, dirIsNeg))
        {
            if (node->nPrimitives > 0)
            {
                // Intersect ray with primitives in leaf BVH node
                for (int i = 0; i < node->nPrimitives; ++i)
                {
                    //find minimum most t value and save isect to be that
                    Intersection resultIsect = Intersection();

                    if (primitives[node->primitivesOffset + i]->Intersect(ray, &resultIsect))
                    {
                        if(resultIsect.t < minimumT)
                        {
                            minimumT = resultIsect.t;
                            minimumIsect = resultIsect;
                            *isect = minimumIsect;
                            hit = true;
                        }
                    }
                }//end for

                if (toVisitOffset == 0) break;      //you're done looking at all the nodes

                currentNodeIndex = nodesToVisit[--toVisitOffset];       //pick next node to look at
            }//end if leaf node

            else
            {
                //either go to the left side or right side of the BST

                // Put far BVH node on _nodesToVisit_ stack, advance to near
                if (dirIsNeg[node->axis])
                {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->secondChildOffset;
                }//end if
                else
                {
                    nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }//end else
            }//end else
        }//end if bvh does intersect

        else
        {
            if (toVisitOffset == 0) break;
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }//end else bvh doesn't intersect
    }//end while

    return hit;
}//end intersect function


int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *offset)
{
    LinearBVHNode *linearNode = &nodes[*offset];

    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;

    if (node->nPrimitives > 0) {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }

    else
    {
        // Create interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flattenBVHTree(node->children[0], offset);
        linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
    }
    return myOffset;
}


BVHBuildNode* BVHAccel::recursiveBuild(std::vector<BVHPrimitiveInfo> &primitiveInfo,
                                        int start, int end, int *totalNodes,
                                        std::vector<std::shared_ptr<Primitive>> &orderedPrims)
{
    BVHBuildNode* outputNode = new BVHBuildNode();
    (*totalNodes)++;

    //BOOK --------------------- compute bounds for all BVH nodes
    Bounds3f nodeBounds;
    for(int i = start; i < end; i++)        //[start, end) dictate the range of indicies of primitives in primitiveInfo array that are part of this node
    {
        nodeBounds = Union(nodeBounds, primitiveInfo[i].bounds);        //bounds for this current node (which can encompass multiple primitives)
    }


    int numPrimitives = end - start;        //number of primitives in this node

    //WHAT IF NUMPRIMITIVES = 0 ??? ---> THIS WILL NEVER HAPPEN BECAUSE YOU WONT TRY TO SPLIT A SCENE WITH ONLY ONE PRIMITIVE. YOULL IMMEDIATELY HIT THE IF CASE BELOW

    if(numPrimitives == 1)                  //BASE CASE: if there's only 1 primitive in this node, then make it a leaf
    {
        //create leaf BVH node
        int firstPrimOffset = orderedPrims.size();      //an offset from where in primitiveInfo array you should start from

        for(int i = start; i < end; i++)        //for the number of primitives (THIS IS WEIRD. ISNT IT JUST TECHNICALLY ONE PRIMITIVE ANYWAYS???)
        {
            int primitiveNum = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[primitiveNum]);
        }
        outputNode->InitLeaf(firstPrimOffset, numPrimitives, nodeBounds);
        return outputNode;
    }//end if

    else        //subdivide bounding area into 2 child nodes and recursively call this function again
    {
        //BOOK --------------------- compute bound of primitive centroids, choose split dimension
        Bounds3f centroidBoundsOfNode;
        for(int i = start; i < end; i++)
        {
            centroidBoundsOfNode = Union(centroidBoundsOfNode, primitiveInfo[i].centroid);
        }

        int splitAxisDimension = centroidBoundsOfNode.MaximumExtent();                  //select axis with largest extent

        //BOOK --------------------- partition primitives into 2 sets and build children
        int mid = (start + end) / 2;

        if(centroidBoundsOfNode.max[splitAxisDimension] == centroidBoundsOfNode.min[splitAxisDimension])    //if all centroid points are in same posiiton (AKA no volume), then make leaf node with primitives that have that centroid position
        {
            //create leaf BVH node
            int firstPrimOffset = orderedPrims.size();      //an offset from where in primitiveInfo array you should start from

            for(int i = start; i < end; i++)        //for the number of primitives (THIS IS WEIRD. ISNT IT JUST TECHNICALLY ONE PRIMITIVE ANYWAYS???)
            {
                int primitiveNum = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[primitiveNum]);
            }
            outputNode->InitLeaf(firstPrimOffset, numPrimitives, nodeBounds);
            return outputNode;
        }

        else
        {
            if(numPrimitives <= 4)
            {
                mid = (start + end) / 2;
                std::nth_element(&primitiveInfo[start], &primitiveInfo[mid], &primitiveInfo[end - 1] + 1,
                        [splitAxisDimension](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b)
                {
                    return a.centroid[splitAxisDimension] < b.centroid[splitAxisDimension];
                });
            }//end if
            else
            {
                /* ========================= BEGIN: SURFACE AREA HEURISTIC ========================= */
                //BOOK --------------------- partiton primitives based on surface area heuristic (AKA find what mid should be)


                // ====================== create a vector of buckets
                int numBuckets = 12;
                std::vector<std::pair<int, Bounds3f>> buckets;
                for(int i = 0; i < numBuckets; i++)
                {
                    buckets.push_back(std::pair<int, Bounds3f>(0, Bounds3f()));
                }

                //initailize buckets for SAH partition
                for(int i = start; i < end; i++)
                {
                    //NOTE: the value of b will always be between 0 and numBuckets
                    int b = numBuckets * centroidBoundsOfNode.Offset(primitiveInfo[i].centroid)[splitAxisDimension];
                    if(b == numBuckets)
                    {
                        b = numBuckets - 1;
                    }

                    buckets[b].first += 1;
                    buckets[b].second = Union(buckets[b].second, primitiveInfo[i].bounds);
                }//end for

                //now we have, for each bucket, count of number of primitives and bounds of their bounding box

                // ====================== compute costs for splitting after each bucket
                std::vector<float> costList;
                for(int i = 0; i < numBuckets - 1; i++)
                {
                    Bounds3f bound0, bound1 = Bounds3f();
                    int count0, count1 = 0;

                    for(int j = 0; j <= i; j++)
                    {
                        bound0 = Union(bound0, buckets[j].second);
                        count0 += buckets[j].first;
                    }

                    for(int k = i + 1; k < numBuckets; k++)
                    {
                        bound1 = Union(bound1, buckets[k].second);
                        count1 += buckets[k].first;
                    }

                    float currCost = .125f * ((count0 + bound0.SurfaceArea()) + (count1 * bound1.SurfaceArea()))
                                            / nodeBounds.SurfaceArea();
                    costList.push_back(currCost);
                }//end for

                // ====================== find bucket to split at that minimizes SAH heuristic
                float minCost = costList[0];
                int minCostPartition = 0;

                for(int i = 1; i < numBuckets - 1; i++)
                {
                    if(costList[i] < minCost)
                    {
                        minCost = costList[i];
                        minCostPartition = i;
                    }//end if
                }//end for


                // ====================== create leaf or split primtives at selected partition / SAH bucket
                float leafCost = numPrimitives;
                if(numPrimitives > maxPrimsInNode || minCost < leafCost)
                {
                    BVHPrimitiveInfo *partitionMid = std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                            [=](const BVHPrimitiveInfo &pi) {
                                    int b = numBuckets * centroidBoundsOfNode.Offset(pi.centroid)[splitAxisDimension];
                                    if(b == numBuckets) b = numBuckets - 1;
                                    return b <= minCostPartition;
                            });

                    mid = partitionMid - &primitiveInfo[0];
                }//end if
                else
                {
                    //create leaf BVH node
                    int firstPrimOffset = orderedPrims.size();      //an offset from where in primitiveInfo array you should start from

                    for(int i = start; i < end; i++)        //for the number of primitives (THIS IS WEIRD. ISNT IT JUST TECHNICALLY ONE PRIMITIVE ANYWAYS???)
                    {
                        int primitiveNum = primitiveInfo[i].primitiveNumber;
                        orderedPrims.push_back(primitives[primitiveNum]);
                    }
                    outputNode->InitLeaf(firstPrimOffset, numPrimitives, nodeBounds);
                    return outputNode;
                }//end else


                /* ========================= END: SURFACE AREA HEURISTIC ========================= */

            }//end else



            BVHBuildNode* child1 = recursiveBuild(primitiveInfo, start, mid, totalNodes, orderedPrims);
            BVHBuildNode* child2 = recursiveBuild(primitiveInfo, mid, end, totalNodes, orderedPrims);
            outputNode->InitInterior(splitAxisDimension, child1, child2);
        }//end else

    }//end else


    return outputNode;

}//end recursiveBuild
