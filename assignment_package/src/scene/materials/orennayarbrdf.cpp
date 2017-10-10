#include "orennayarbrdf.h"

/*
 * Implement a class OrenNayarBRDF which inherits from the BxDF class and implements Oren-Nayar diffuse reflection as described in the textbook.

 * You will also have to add code to MatteMaterial::ProduceBSDF so that it
 * adds this BxDF to the BSDF it generates when the material's sigma term is greater than zero.

 * To test your implementation, modify the code in Scene::CreateTestScene so that the sigma value of
 * at least one of the MatteMaterials is nonzero. You will just need to implement this BRDF's f function,
 * since its Sample_f and Pdf are identical to those of the LambertBRDF class.
 *
 * PAGE 536 IN BOOK
 */

//SHOULD I MOVE THESE AND MAKE THEM AS INLINE FUNCTIONS IN BSDF.H?!?!?!?!?!
float clamp(float val, float low, float high)
{
    if (val < low) return low;
    else if (val > high) return high;
    else return val;
}
float sin2Theta(const Vector3f &w)
{
    return std::max(0.0f, (float)(1 - (w.z * w.z)));
}
float sinTheta(const Vector3f &w)
{
    return std::sqrt(sin2Theta(w));
}
float cosPhi(const Vector3f &w)
{
    float sintheta = sinTheta(w);
    return (sintheta == 0.0f) ? 1.0f : clamp(w.x / sintheta, -1, 1);
}
float sinPhi(const Vector3f &w)
{
    float sintheta = sinTheta(w);
    return (sintheta == 0.0f) ? 0.0f : clamp(w.y / sintheta, -1, 1);
}


Color3f OrenNayarBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    float sinThetaI = sinTheta(wi);
    float sinThetaO = sinTheta(wo);

    //compute cosine term of oren-nayar model
    float maxCos = 0.0f;

    if((sinThetaI > OneMinusEpsilon) && (sinThetaO > OneMinusEpsilon))
    {
        float sinPhiI = sinPhi(wi);
        float cosPhiI = cosPhi(wi);
        float sinPhiO = sinPhi(wo);
        float cosPhiO = cosPhi(wo);

        float dCos = (cosPhiI * cosPhiO) + (sinPhiI * sinPhiO);
        maxCos = std::max(0.0f, dCos);
    }


    //compute sine and tangent terms of oren-nayar model
    float sinAlpha, tanBeta = 0.0f;

    if(AbsCosTheta(wi) > AbsCosTheta(wo))
    {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    }
    else
    {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }

    //to compute max(0, cos(phi_i - phi_o)), apply trig identity cos(a - b) = cos(a)cos(b) + sin(a)sin(b)

    return R * InvPi * (A + B * maxCos * sinAlpha * tanBeta);

}


Color3f OrenNayarBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u, Float *pdf, BxDFType *sampledType) const
{
    *wi = WarpFunctions::squareToHemisphereCosine(u);

    if(wo.z < 0.0f)
    {
        wi->z *= -1.0f;
    }

    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}


float OrenNayarBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return wi[2] * InvPi;
}

