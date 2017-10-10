#pragma once

#include <scene/lights/light.h>

//PAGE 731 IN BOOK
class DistantLight : public Light
{
public:
    DistantLight(const Transform &t, const Color3f &L, const Vector3f &wLight)
        : Light(t, false, true),
          Le(L),
          wLight(glm::normalize(wLight))
    {}

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const;

    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;


    virtual std::shared_ptr<Shape> getShape() const;

private:
    const Color3f Le;
    const Vector3f wLight;

//    Point3f worldCenter;
//    Float worldRadius;

};

