#include "distantlight.h"


std::shared_ptr<Shape> DistantLight::getShape() const
{
    //do i need to return anything here?
}


Color3f DistantLight::L(const Intersection &isect, const Vector3f &w) const
{
    return Le * glm::clamp(glm::dot(-wLight, w), 0.0f, 1.0f);
}


Color3f DistantLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = -wLight;
    *pdf = 1.0f;
    return Le;
}


Float DistantLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
