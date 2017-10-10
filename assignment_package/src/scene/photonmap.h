#pragma once

#include <scene/scene.h>
#include <globals.h>
#include <samplers/sampler.h>
#include <warpfunctions.h>

#include <unordered_map>

//struct PhotonSample;
struct PhotonSample
{
    PhotonSample()
        : point(Point3f(0.0f)), direction(Vector3f(0.0f)), power(Color3f(0.0f)) {}

    PhotonSample(Point3f point, Vector3f direction, Color3f power)
        : point(point), direction(direction), power(power) {}

    Point3f point;
    Vector3f direction;
    Color3f power;
};


class PhotonMap
{
public:
    PhotonMap(Scene s, int recursionLimit, std::shared_ptr<Sampler> sampler, int numPhotons);

    //helper functions
//    void sampleRay(Intersection firstIsect, Vector3f fromPrimaryIsect_wi, Color3f accumulatedColor);
    void addToMap(std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> &map, PhotonSample photon, std::tuple<int, int, int> voxelGridCoord);
    //Color3f getColor(Point3f point, float radius, std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> map);      //averages color of photons in a map within a radius
    Color3f getColor(Intersection isect, float radius, int mapFlag);
    void fillPhotonMap(std::vector<PhotonSample>* photons, Point3f &pt, std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> &map);


private:
    Scene scene;
    std::shared_ptr<Sampler> sampler;
    int recursionLimit;
    int numPhotons;

    //map --> key: grid cell lower left coord, value: vector of photon samples
    std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> causticMap;
    std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> indirectMap;




    //std::unordered_map<std::tuple<int, int, int>, std::vector<PhotonSample>> unordered_causticMap;
    //std::map<std::tuple<int, int, int>, std::vector<PhotonSample>> indirectMap;

//    struct MapHasher {
//        std::size_t operator()(const std::tuple<int, int, int> &val, ) const
//        {}
//    };
//    typedef std::unordered_set<Move, MoveHasher> Set;




};


//CHANGE TO USE UNORDERED MAP OR TRY USING QMAP
//http://stackoverflow.com/questions/21288345/unordered-set-of-pairs-compilation-error
//http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
//http://doc.qt.io/qt-5/qmap.html

