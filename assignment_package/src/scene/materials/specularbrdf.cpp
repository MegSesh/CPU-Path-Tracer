#include "specularbrdf.h"

Color3f SpecularBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO!

    //figure out which n is incident and which is transmitted
    //compute ray direction for specular transmission
    //pg 529 in book
    //costheta --> returns z component of input vector
    //Color3f ft = R * (Color3f(1.0f) - fresnel->Evaluate(*wi->z));
    //account for non-symmetry with transmission to different medium

    //BRDF
    //compute perfect specular reflection direction

    *wi = Vector3f(-wo.x, -wo.y, wo.z);
    *pdf = 1.0f;
    return (fresnel->Evaluate(CosTheta(*wi)) * R) / AbsCosTheta(*wi);


    //(fresnel->Evaluate(CosTheta(*wi)) * R) / AbsCosTheta(*wi);
    //(fresnel->Evaluate(wi->z) * R) / glm::abs(wi->z);
}
