#pragma once

#include <scene/lights/light.h>

//PAGE 721 IN BOOK
class SpotLight : public Light
{
public:
    SpotLight(const Transform &t, const Color3f& I, Float totalWidth, Float fallOffStart)
        : Light(t, true, false),
          pLight(t.position()),
          I(I),
          cosTotalWidth(std::cos((Pi / 180) * totalWidth)),
          cosFallOffStart(std::cos((Pi / 180) * fallOffStart))
    {}

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const;

    Float FallOff(const Vector3f &w) const;

    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;


    virtual std::shared_ptr<Shape> getShape() const;


private:
    const Point3f pLight;
    const Color3f I;
    const Float cosTotalWidth;
    const Float cosFallOffStart;

};
