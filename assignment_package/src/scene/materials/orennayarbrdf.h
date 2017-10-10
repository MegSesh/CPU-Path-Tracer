//#ifndef ORENNAYARBRDF_H
//#define ORENNAYARBRDF_H
#pragma once
#include "bsdf.h"
#include "warpfunctions.h"


class OrenNayarBRDF : public BxDF
{
public:
    OrenNayarBRDF(const Color3f &R, float sigma)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
    {
        sigma = glm::radians(sigma);
        float sigma2 = sigma * sigma;
        A = 1.0f - (sigma2 / (2.0f * (sigma2 + 0.33f)));
        B = 0.45f * sigma2 / (sigma2 + 0.09f);
    }

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;

    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;


private:
    const Color3f R;
    Float A, B;

};

//#endif // ORENNAYARBRDF_H
