#include "lambertbrdf.h"
#include "warpfunctions.h"


/*
 * In scene/materials/lambertbrdf.h you will find the declaration of a class LambertBRDF,
 * which will be used to evaluate the scattering of energy by perfectly Lambertian surfaces.
 * For this class, you must implement the f, Sample_f, and Pdf functions.


 * Unlike the generic BxDF class, the LambertBRDF class will:
 * generate cosine-weighted samples within the hemisphere in its Sample_f and
 * will return the PDF associated with this distribution in Pdf.
 * Most importantly, f must return the proportion of energy from wi that is reflected along wo for a Lambertian surface.
 * This proportion will include the energy scattering coefficient of the BRDF, R.
 *
 *
 * LOOK AT PIAZZA POST 137
 * https://piazza.com/class/ixl1kngw3id1ab?cid=137
 */


Color3f LambertBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    //return Color3f(0.f);

    return R * InvPi;
}

Color3f LambertBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //return BxDF::Sample_f(wo, wi, u, pdf, sampledType);

    //Using the warping function from homework 3, warp a single 2D point onto the surface of a hemisphere
    //IF THIS GETS EDITED, MAKE SURE TO CHANGE OREN NAYAR AND LAMB TRANSMISSION
    *wi = WarpFunctions::squareToHemisphereCosine(u);//squareToHemisphereUniform(u);

    if(wo.z < 0.0f)
    {
        wi->z *= -1.0f;
    }

    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float LambertBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    //return BxDF::Pdf(wo, wi);

    //IF THIS GETS EDITED, MAKE SURE TO CHANGE OREN NAYAR AND LAMB TRANSMISSION

    //return wi[2] * InvPi;
    if(SameHemisphere(wo, wi))
    {
        return AbsCosTheta(wi) * InvPi;
    }
    else
    {
        return 0.0f;
    }
}
