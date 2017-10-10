#include "microfacetbrdf.h"

/*
 * Implement the f, Pdf, and Sample_f functions of the MicrofacetBRDF class so they follow the implementation of a Torrance-Sparrow microfacet BRDF.
 * The MirrorMaterial and PlasticMaterial classes make use of this BRDF to create glossy surfaces.

 * Note that this BRDF class makes use of a MicrofacetDistribution pointer for some of the functions mentioned above.

 * We have provided you with the implementation of a Trowbridge-Reitz
 * microfacet distribution for use with your MicrofacetBRDF class functions;
 * as noted in the extra credit section, you may implement additional microfacet distributions if you wish.

 * Refer to microfacet.h for brief explanations of the functions of this class.

 * PAGE 547 AND 811 - 813 IN BOOK
 * D and G1 --> PAGE 538, 542
 * SampleVisibleArea --> 808

 */
Color3f MicrofacetBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO!

    Color3f output = Color3f(0.0f);

    Float cosThetaO = AbsCosTheta(wo);
    Float cosThetaI = AbsCosTheta(wi);

    Vector3f wh = wi + wo;

    //handle degenerate cases for microfacet reflection
    if(cosThetaI == 0.0f || cosThetaO == 0.0f)
    {
        return Color3f(0.0);
    }
    if(wh.x == 0.0f && wh.y == 0.0f && wh.z == 0.0f)
    {
        return Color3f(0.0f);
    }

    wh = glm::normalize(wh);
    Color3f F = fresnel->Evaluate(glm::dot(wi, wh));
    output = R * distribution->D(wh) * distribution->G(wo, wi) * F / (4.0f * cosThetaI * cosThetaO);

    return output;
}

Color3f MicrofacetBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //sample microfacet orientation wh and reflected direction wi
    Vector3f wh = distribution->Sample_wh(wo, xi);
    *wi = glm::reflect(-wo, wh); //wo should be negated, book doesn't
    if(!SameHemisphere(wo, *wi))
    {
        return Color3f(0.0f);
    }

    //compute pdf of wi for microfacet reflection
    *pdf = distribution->Pdf(wo, wh) / (4.0f * glm::dot(wo, wh));

    return f(wo, *wi);
}

float MicrofacetBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    if(!SameHemisphere(wo, wi))
    {
        return 0.0f;
    }

    Vector3f wh = glm::normalize(wo + wi);
    return distribution->Pdf(wo, wh) / (4.0f * glm::dot(wo, wh));
}
