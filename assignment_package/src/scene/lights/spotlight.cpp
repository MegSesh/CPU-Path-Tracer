#include "spotlight.h"

std::shared_ptr<Shape> SpotLight::getShape() const
{
    //do i need to return anything here?
}

Color3f SpotLight::L(const Intersection &isect, const Vector3f &w) const
{
    return Color3f(0.0f);
}


Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi, Vector3f *wi, Float *pdf) const
{
    *wi = glm::normalize(pLight - ref.point);
    *pdf = 1.0f;
    return I * FallOff(-*wi) / glm::length2(pLight - ref.point);
}


Float SpotLight::FallOff(const Vector3f &w) const
{
    Vector3f wl = glm::vec3(glm::normalize(transform.invT() * glm::vec4(w, 0.0f)));
    Float cosTheta = wl.z;

    if(cosTheta < cosTotalWidth)
    {
        return 0.0f;
    }
    if(cosTheta > cosFallOffStart)
    {
        return 1.0f;
    }

    Float delta = (cosTheta - cosTotalWidth) / (cosFallOffStart - cosTotalWidth);

    return (delta * delta) * (delta * delta);
}


Float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}
