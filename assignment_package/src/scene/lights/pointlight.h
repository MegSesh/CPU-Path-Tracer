#pragma once

#include <scene/lights/light.h>

class PointLight : public Light
{
public:
    PointLight(const Transform &t, const Color3f &I)
        : Light(t, true, false),
          pLight(t.position()),    //glm::vec3(transform.T() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
          I(I)
    {}


    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    Color3f Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const;

    virtual Float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;


    virtual std::shared_ptr<Shape> getShape() const;


private:
    const Point3f pLight;
    const Color3f I;
};
