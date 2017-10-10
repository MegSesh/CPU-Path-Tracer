#include "fresnel.h"

/*
 * As such, you must implement the Evaluate function of the FresnelDielectric class so that it correctly computes
 * how reflective a surface point on a dielectric material should be given its indices of refraction and the angle
 * between the incident ray and the surface normal.
 */
Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO
    //return Color3f(0.f);
    //page 519 - 523

    //return FrDielectric(cosThetaI, etaI, etaT);

    float clampedCTI = glm::clamp(cosThetaI, -1.0f, 1.0f);

    //potentially swap indices of refraction
    Float _etaI = etaI;     //without saving these private variables here, i got a "read only" error
    Float _etaT = etaT;
    bool entering = clampedCTI > 0.0f;
    if(!entering)
    {
        //std::swap(etaI, etaT);
        Float temp = _etaI;
        _etaI = _etaT;
        _etaT = temp;

        clampedCTI = std::abs(clampedCTI);
    }

    //compute cosThetaT using snell's law
    Float sinThetaI = std::sqrt(std::max(Float(0), Float(1 - clampedCTI * clampedCTI)));
    Float sinThetaT = (_etaI / _etaT) * sinThetaI;
    // Handle total internal reflection for transmission
    if (sinThetaT >= 1.0f) return Color3f(0.0f);   //return 1.f
    Float cosThetaT = std::sqrt(std::max(Float(0), 1 - sinThetaT * sinThetaT));


    Float Rpar1 = ((_etaT * clampedCTI) - (_etaI * cosThetaT)) /
                    ((_etaT * clampedCTI) + (_etaI * cosThetaT));

    Float Rperp = ((_etaI * clampedCTI) - (_etaT * cosThetaT)) /
                    ((_etaI * clampedCTI) + (_etaT * cosThetaT));

    Float result = ((Rpar1 * Rpar1) + (Rperp * Rperp)) / 2.0f;

    return Color3f(result);

}
