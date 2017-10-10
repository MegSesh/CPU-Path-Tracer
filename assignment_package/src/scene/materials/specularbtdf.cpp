#include "specularbTdf.h"

Color3f SpecularBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO!

    /*
     * Like for the SpecularBRDF class, you must implement the Sample_f function of the SpecularBTDF class.

     * This function is a little more involved, however, as you not only must generate ωi by refracting ωo,
     * but you must also check for total internal reflection and return black if it would occur.

     * Remember, this class only handles transmission, so it should not compute the color that would be
     * returned by total internal reflection if it were to be simulated.

     * You must also make sure to check whether your ray is entering or leaving the object with which it has intersected;
     * you can do this by comparing the direction of ωo to the direction of your normal (remember, you are in tangent space so this is pretty easy).

     * If your ray is leaving a transmissive object, you should compute its index of refraction as etaB / etaA rather than the other way around.

     */

    //figure out which n is incident and which is transmitted
    bool entering = CosTheta(wo) > 0.0f;//wo.z > 0.0f;
    float etaI = entering ? etaA : etaB;
    float etaT = entering ? etaB : etaA;

    //compute ray direction for specular transmission
    //takes care of total internal reflection
    if(!Refract(wo, Faceforward(Normal3f(0.0f, 0.0f, 1.0f), wo), etaI / etaT, wi))
    {
        return Color3f(0.0f);
    }

    *pdf = 1.0f;

    Color3f ft = T * (Color3f(1.0f) - fresnel->Evaluate(wi->z));

    //account for non-symmetry with transmission to different medium
    //transportmode indicates whether indicident ray that intersected point where BxDF was computed started from a light source or camera
//    if(mode == TransportMode::Radiance)
//    {
//        ft *= (etaI * etaI) / (etaT * etaT);
//    }


    return ft / AbsCosTheta(*wi);//glm::fabs(wi->z);
}
