#include "lambertbtdf.h"
#include "warpfunctions.h"

/*
 * Implement a class LambertBTDF which inherits from the BxDF class and implements a Lambertian transmission model.
 * This is virtually identical to a Lambertian reflection model, but the hemisphere in which rays are sampled
 * is on the other side of the surface normal compared to the hemisphere of Lambertian reflection.
 */

Color3f LambertBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return T * InvPi;
}


Color3f LambertBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u, Float *pdf, BxDFType *sampledType) const
{
    *wi = WarpFunctions::squareToHemisphereCosine(u);

    if(wo.z > 0.0f)
    {
        wi->z *= -1.0f;
    }

    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}


float LambertBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //return wi[2] * InvPi;
    if(!SameHemisphere(wo, wi))
    {
        return AbsCosTheta(wi) * InvPi;
    }
    else
    {
        return 0.0f;
    }
}
