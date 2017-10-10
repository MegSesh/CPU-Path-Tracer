#pragma once
#include "integrator.h"
#include <scene/photonmap.h>


class PhotonMapIntegrator : public Integrator
{
public:
    PhotonMapIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit, PhotonMap &photonMap)
        : Integrator(bounds, s, sampler, recursionLimit), photonMap(photonMap)
    {}

    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundedEnergy) const;

    float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const;
    float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const;

private:
    PhotonMap &photonMap;

};
