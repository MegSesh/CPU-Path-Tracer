#pragma once
#include "bsdf.h"
#include "microfacet.h"
#include "fresnel.h"

//PAGE 548 and 813 IN BOOK
class MicrofacetBTDF : public BxDF
{
public:
    MicrofacetBTDF(const Color3f &T, MicrofacetDistribution* distribution, Float etaA, Float etaB)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)), T(T), distribution(distribution),
          etaA(etaA), etaB(etaB), fresnel(etaA, etaB) {}

    virtual ~MicrofacetBTDF(){ delete distribution; }

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                             const Point2f &xi, Float *pdf,
                             BxDFType *sampledType = nullptr) const;

    virtual Float Pdf(const Vector3f &wo, const Vector3f &wi) const;


private:
    const Color3f T;
    const MicrofacetDistribution* distribution;
    const Float etaA, etaB;
    const FresnelDielectric fresnel;
};
