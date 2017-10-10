#include "photonmap.h"

#include "scene/lights/diffusearealight.h"

//struct PhotonSample
//{
//    PhotonSample()
//        : point(Point3f(0.0f)), direction(Vector3f(0.0f)), power(Color3f(0.0f)) {}

//    PhotonSample(Point3f point, Vector3f direction, Color3f power)
//        : point(point), direction(direction), power(power) {}

//    Point3f point;
//    Vector3f direction;
//    Color3f power;
//};



PhotonMap::PhotonMap(Scene s, int recursionLimit, std::shared_ptr<Sampler> sampler, int numPhotons)
    : scene(s), recursionLimit(recursionLimit), sampler(sampler), numPhotons(numPhotons)

{
    int numLights = scene.lights.size();
    if(numLights == 0 || numPhotons <= 0)   return;

    for(int j = 0; j < numPhotons; j++)
    {
        for(int i = 0; i < numLights; i++)
        {
            bool prevSpecular = false;
            bool firstNonSpec = true;

            Color3f accumulatedColor(1.0f);
            int depth = recursionLimit;

            //origin and direction of fromLight ray is dependent on light shape
            Vector3f t(0.0f);
            Vector3f b(0.0f);
            Normal3f n(0.0f);
            Point3f fromLight_origin = scene.lights.at(i)->getShape()->sampleSurface(sampler, &t, &b, &n);
            Vector3f fromLight_direction = glm::mat3(t, b, n) * WarpFunctions::squareToHemisphereUniform(sampler->Get2D());

            //offset the origin point along normal
            Vector3f originOffset = n * RayEpsilon;
            originOffset = (glm::dot(fromLight_direction, n) > 0) ? originOffset : -originOffset;
            Ray fromLight_ray = Ray(fromLight_origin + originOffset, glm::normalize(fromLight_direction));


            //first intersection with object in scene from light
            Intersection firstIsect = Intersection();
            bool intersectionExists = scene.Intersect(fromLight_ray, &firstIsect);

            if(!intersectionExists)   break;
            if(!firstIsect.ProduceBSDF())   break;

            Vector3f fromPrimaryIsect_wi = Vector3f(0.0f);
            float fromPrimaryIsect_pdf = 0.0f;
            BxDFType fromPrimaryIsect_type = BSDF_ALL;
            BxDFType fromPrimaryIsect_sampledType;
            Color3f fromPrimaryIsect_sample_f = firstIsect.bsdf->Sample_f(-fromLight_ray.direction, &fromPrimaryIsect_wi, sampler->Get2D(), &fromPrimaryIsect_pdf, fromPrimaryIsect_type, &fromPrimaryIsect_sampledType);

            if(fromPrimaryIsect_pdf == 0.0f)    break;

            if(fromPrimaryIsect_sampledType & BSDF_SPECULAR)
            {
                prevSpecular = true;
                //firstNonSpec = false;
            }

            //update throughput with first isect's sample f, abs dot, and pdf
            accumulatedColor *= (fromPrimaryIsect_sample_f * AbsDot(fromPrimaryIsect_wi, firstIsect.normalGeometric)) / fromPrimaryIsect_pdf;           //scene.lights.at(i)->Pdf_Li(firstIsect, fromPrimaryIsect_wi);

            Ray firstIsect_ray = firstIsect.SpawnRay(fromPrimaryIsect_wi);

            while(depth > 0)
            {
                Ray ith_ray = firstIsect_ray;
                Intersection ith_intersection = Intersection();
                bool ithIntersectionExists = scene.Intersect(ith_ray, &ith_intersection);

                if(!ithIntersectionExists)  break;
                if(!ith_intersection.ProduceBSDF())  break;

                Vector3f ith_wi = Vector3f(0.0f);
                float ith_pdf = 0.0f;
                BxDFType ith_type = BSDF_ALL;
                BxDFType ith_sampledType;

                Color3f ith_sample_f = ith_intersection.bsdf->Sample_f(-ith_ray.direction, &ith_wi, sampler->Get2D(), &ith_pdf, ith_type, &ith_sampledType);

                if(ith_pdf == 0.0f) break;

                if(!(ith_sampledType & BSDF_SPECULAR))
                {
                    if(prevSpecular && firstNonSpec)
                    {
                        accumulatedColor *= (ith_sample_f * AbsDot(ith_wi, ith_intersection.normalGeometric)) / ith_pdf;

                        PhotonSample photon = PhotonSample(ith_intersection.point, ith_wi, accumulatedColor);
                        std::tuple<int, int, int> gridCoord(std::floor(ith_intersection.point.x),
                                                            std::floor(ith_intersection.point.y),
                                                            std::floor(ith_intersection.point.z));

                        addToMap(causticMap, photon, gridCoord);

                        prevSpecular = false;
                        firstNonSpec = false;
                    }
                    else// if(!firstNonSpec)
                    {
                        accumulatedColor *= (ith_sample_f * AbsDot(ith_wi, ith_intersection.normalGeometric)) / ith_pdf;

                        PhotonSample photon = PhotonSample(ith_intersection.point, ith_wi, accumulatedColor);
                        std::tuple<int, int, int> gridCoord(std::floor(ith_intersection.point.x),
                                                            std::floor(ith_intersection.point.y),
                                                            std::floor(ith_intersection.point.z));

                        addToMap(indirectMap, photon, gridCoord);

                        prevSpecular = false;
                    }

                }
                else
                {
                    prevSpecular = true;
                }


                //spawn ray ith's isect's w_i
                Ray nextRay = ith_intersection.SpawnRay(ith_wi);
                firstIsect_ray = nextRay;

                //Russian roulette
                float randRouletteNum = sampler->Get1D();
                float maxComponent = glm::max(glm::max(accumulatedColor.x, accumulatedColor.y), accumulatedColor.z);
                if(depth <= recursionLimit - 3)
                {
                    if(maxComponent < randRouletteNum)  break;
                    else {accumulatedColor /= maxComponent;}
                }

                depth--;
            }//end while
        }//end for every light
    }//end for numPhotons

}//end constructor


void PhotonMap::addToMap(std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> &map, PhotonSample photon, std::tuple<int, int, int> voxelGridCoord)
{
    auto iterator = map.find(voxelGridCoord);
    if(iterator == map.end())    //voxel grid doesn't already exist in map. add new pair.
    {
        std::vector<PhotonSample> photons;
        photons.push_back(photon);
        map.insert(std::pair<std::tuple<int, int, int>, std::vector<PhotonSample>> (voxelGridCoord, photons));
    }
    else
    {
        //add new photon to vector of corresponding key
        map.at(voxelGridCoord).push_back(photon);
    }
}//end addToMap function


Color3f PhotonMap::getColor(Intersection isect, float radius, int mapFlag)
{
    Color3f totalColor(0.0f);
    int numPhotons = 0;

    //find which map to use
    std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> map;
    map = (mapFlag == 0) ? causticMap : indirectMap;

    int floor_x = std::floor(isect.point.x);
    int floor_y = std::floor(isect.point.y);
    int floor_z = std::floor(isect.point.z);

    for(int x = floor_x - 1; x <= floor_x + 1; x++)
    {
        for(int y = floor_y - 1; y <= floor_y + 1; y++)
        {
            for(int z = floor_z - 1; z <= floor_z + 1; z++)
            {
                std::tuple<int, int, int> curr_pt(x, y, z);
                auto iterator = map.find(curr_pt);
                if(!(iterator == map.end()))
                {
                    std::vector<PhotonSample> photonList = map.at(curr_pt);
                    for(int i = 0; i < photonList.size(); i++)
                    {
                        float dist = glm::length(isect.point - photonList.at(i).point);
                        if(dist <= radius)
                        {
                            //(1-t)a + tb
                            //weight = (search radius - length of vector) / radius
                            //weight makes it darker
                            //multiply weight to color and then add
                            float weight = 1.0f;//(radius - dist) / radius;
                            totalColor += photonList.at(i).power * weight;
                            numPhotons++;
                        }//end if
                    }//end for
                }//end if
            }//end z
        }//end y
    }//end x


    if(!IsBlack(totalColor))    //need to check cuz caustic map might be empty
    {
        totalColor /= numPhotons;
    }

    return totalColor;


}//end getColor function


void PhotonMap::fillPhotonMap(std::vector<PhotonSample>* photons, Point3f &pt, std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> &map)
{
    std::tuple<int, int, int> tup_pt1(std::floor(pt.x), std::floor(pt.y), std::floor(pt.z));
    auto iterator1 = map.find(tup_pt1);
    if(iterator1 != map.end())
    {
        std::vector<PhotonSample> photonList = map.at(tup_pt1);
        for(int i = 0; i < photonList.size(); i++)
        {
            photons->push_back(photonList.at(i));
        }
    }
}
