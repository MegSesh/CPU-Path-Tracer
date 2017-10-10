//#ifndef LAMBERTBTDF_H
//#define LAMBERTBTDF_H
#pragma once
#include "bsdf.h"



class LambertBTDF : public BxDF
{
public:
    LambertBTDF(const Color3f &T)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), T(T) {}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

    virtual Color3f Sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampledType = nullptr) const;

    virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const;

private:
    const Color3f T;
};

//#endif // LAMBERTBTDF_H
