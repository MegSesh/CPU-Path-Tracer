#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/lights/light.h>
#include <scene/geometry/shape.h>
#include "bvh.h"

#include <scene/projectivecamera.h>

class Primitive;
class BVHAccel;
class Material;
class Light;

class Scene
{
public:
    Scene();
    ~Scene();
    QList<std::shared_ptr<Primitive>> primitives;
    QList<std::shared_ptr<Material>> materials;
    QList<std::shared_ptr<Light>> lights;

    Camera camera;//make this into a pointer??

    Film film;

    BVHAccel* bvh;

    QList<std::shared_ptr<Drawable>> drawables;

    void SetCamera(const Camera &c);

    //created this for projective camera
    void SetProjectiveCamera(const Camera &c, const Float lensRadius, const Float focalDistance);

    void CreateTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;

    void clearBVH();
};
