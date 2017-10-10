#pragma once
#include "material.h"

class MicrofacetTransmissiveMaterial : public Material
{
public:
    MicrofacetTransmissiveMaterial(const Color3f &Kt, float roughness, float indexOfRefraction,
                                   const std::shared_ptr<QImage> &roughnessMap,
                                   const std::shared_ptr<QImage> &textureMap,
                                   const std::shared_ptr<QImage> &normalMap)
        :Kt(Kt), roughness(roughness), indexOfRefraction(indexOfRefraction),
          roughnessMap(roughnessMap), textureMap(textureMap), normalMap(normalMap)
    {}

    void ProduceBSDF(Intersection *isect) const;

private:
    Color3f Kt;         //transmission of this material (it's base color)

    float roughness;

    float indexOfRefraction;

    std::shared_ptr<QImage> roughnessMap;
    std::shared_ptr<QImage> textureMap;
    std::shared_ptr<QImage> normalMap;

};
