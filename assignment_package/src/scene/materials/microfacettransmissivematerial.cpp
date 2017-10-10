#include "microfacettransmissivematerial.h"
#include "microfacet.h"
#include "microfacetbtdf.h"

void MicrofacetTransmissiveMaterial::ProduceBSDF(Intersection *isect) const
{
    isect->bsdf = std::make_shared<BSDF>(*isect, indexOfRefraction);

    Color3f color = Kt;
    if(this->textureMap)
    {
        color *= Material::GetImageColor(isect->uv, this->textureMap.get());
    }
    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        //Update bsdf's TBN matrices to support the new normal
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);
        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    float rough = roughness;
    if(this->roughnessMap)
    {
        // Gets the greyscale value of the roughness map image and attenuates it by the
        // overall roughness of this material
        Color3f roughRGB = Material::GetImageColor(isect->uv, this->roughnessMap.get());
        rough *= (0.299 * roughRGB.r + 0.587 * roughRGB.g + 0.114 * roughRGB.b);
    }


    //isect->bsdf->Add(new MicrofacetBRDF(color, distrib, new FresnelNoOp()));
    //glass material : isect->bsdf->Add(new SpecularBTDF(transmitColor, 1.f, indexOfRefraction, new FresnelDielectric(1.f, indexOfRefraction)));
    //transmissive material: isect->bsdf->Add(new SpecularBTDF(color, 1.f, indexOfRefraction, new FresnelNoReflect()));


    //Convert roughness from 0,1 scale to alpha term in Trowbridge-Reitz distribution
    rough = RoughnessToAlpha(rough);
    MicrofacetDistribution* distrib = new TrowbridgeReitzDistribution(rough, rough);

    //MicrofacetDistribution* distrib = new BeckmannDistribution(rough, rough);     //DOESNT WORK WITH BECKMANN

    isect->bsdf->Add(new MicrofacetBTDF(color, distrib, 1.0f, indexOfRefraction));

}
