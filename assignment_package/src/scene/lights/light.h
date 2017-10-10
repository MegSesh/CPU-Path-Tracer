#pragma once
#include <globals.h>
#include <scene/transform.h>
#include <raytracing/intersection.h>

#include <scene/geometry/shape.h>
class Shape;

class Intersection;

class Light
{
  public:
    virtual ~Light(){}
    Light(Transform t, bool isPoint, bool isDistant)
        : transform(t), name(),
          isPoint(isPoint), isDistant(isDistant), lightPos(t.position())
    {}

    // Returns the light emitted along a ray that does
    // not hit anything within the scene bounds.
    // Necessary if we want to support things like environment
    // maps, or other sources of light with infinite area.
    // The default implementation for general lights returns
    // no energy at all.
    virtual Color3f Le(const Ray &r) const;

    virtual Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                                Vector3f *wi, Float *pdf) const = 0;


    //NEED TO INCLUDE THIS HERE, BECAUSE NEED IT IN FULL LIGHTING INTEGRATOR
    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const = 0;


    QString name; // For debugging

    //for other light types
    virtual Color3f L(const Intersection &isect, const Vector3f &w) const = 0;

    bool isPoint;
    bool isDistant;
    Point3f lightPos;


    virtual std::shared_ptr<Shape> getShape() const = 0;


  protected:
    const Transform transform;
};

class AreaLight : public Light
{
public:
    AreaLight(const Transform &t) : Light(t, false, false){}
    // Returns the light emitted from a point on the light's surface _isect_
    // along the direction _w_, which is leaving the surface.
    virtual Color3f L(const Intersection &isect, const Vector3f &w) const = 0;

    //NEED TO INCLUDE THIS HERE, BECAUSE NEED IT IN FULL LIGHTING INTEGRATOR
    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const = 0;
};
