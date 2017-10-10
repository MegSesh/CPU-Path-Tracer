#include "microfacetbtdf.h"

Color3f MicrofacetBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    Color3f output = Color3f(0.0f);

    if(SameHemisphere(wo, wi)) return Color3f(0.0f);

    float cosThetaO = CosTheta(wo);
    float cosThetaI = CosTheta(wi);
    if(cosThetaI == 0.0f || cosThetaO == 0.0f) return Color3f(0.0f);

    //compute wh from wo and wi for microfacet transmission
    float eta = CosTheta(wo) > 0 ? (etaB / etaA) : (etaA / etaB);
    Vector3f wh = glm::normalize(wo + wi * eta);

    if(wh.z < 0.0f) wh = -wh;

    Color3f F = fresnel.Evaluate(glm::dot(wo, wh));

    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);

    output = (Color3f(1.0f) - F) * T *
            std::abs(distribution->D(wh) * distribution->G(wo, wi) * eta * eta *
                     AbsDot(wi, wh) * AbsDot(wo, wh) / (cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));

    return output;

}


Color3f MicrofacetBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    Color3f output = Color3f(0.0f);

    if(wo.z == 0.0f)
    {
        return Color3f(0.0f);
    }

    Vector3f wh = distribution->Sample_wh(wo, xi);
    float eta = CosTheta(wo) > 0.0f ? (etaA / etaB) : (etaB / etaA);

    if(!Refract(wo, (Normal3f)wh, eta, wi))
    {
        return Color3f(0.0f);
    }

    *pdf = Pdf(wo, *wi);
    output = f(wo, *wi);

    return output;
}


Float MicrofacetBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    float output = 0.0f;

    if(SameHemisphere(wo, wi))
    {
        return 0.0f;
    }

    //compute wh from wo and wi for microfacet transmission
    float eta = CosTheta(wo) > 0.0f ? (etaB / etaA) : (etaA / etaB);
    Vector3f wh = glm::normalize(wo + wi * eta);


    //compute change of variables dwh_dwi for microfacet transmission
    float sqrtDenom = glm::dot(wo, wh) + eta * glm::dot(wi, wh);
    float dwh_dwi = std::abs((eta * eta * glm::dot(wi, wh)) / (sqrtDenom * sqrtDenom));

    output = distribution->Pdf(wo, wh) * dwh_dwi;
    return output;

}
