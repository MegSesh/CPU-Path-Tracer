#include "pointlight.h"

std::shared_ptr<Shape> PointLight::getShape() const
{
    //do i need to return anything here?
}


Color3f PointLight::L(const Intersection &isect, const Vector3f &w) const
{
    return Color3f(0.0f);
}


Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.0f;

    return I / glm::length2(pLight - ref.point);
}



Float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
