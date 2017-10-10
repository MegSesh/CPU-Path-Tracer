#include "bsdf.h"
#include <warpfunctions.h>

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
//TODO: Properly set worldToTangent and tangentToWorld
    : worldToTangent(glm::mat3(0.0f)),///*COMPUTE ME*/),
      tangentToWorld(glm::mat3(0.0f)),///*COMPUTE ME*/),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{
    UpdateTangentSpaceMatrices(isect.normalGeometric, isect.tangent, isect.bitangent);
}

//WROTE THIS ACCORDING TO PIAZZA POST 183
BSDF::~BSDF()
{
    for(int i = 0; i < numBxDFs; i++)
    {
        delete bxdfs[i];
    }
}


void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    //TODO: Update worldToTangent and tangentToWorld based on the normal, tangent, and bitangent

    tangentToWorld = Matrix3x3(t, b, n);
    worldToTangent = Matrix3x3(glm::transpose(tangentToWorld));
}



Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    //TODO
    //return Color3f(0.f);

    Vector3f wi = worldToTangent * wiW;
    Vector3f wo = worldToTangent * woW;

    bool reflect = glm::dot(wiW, normal) * glm::dot(woW, normal) > 0;

    Color3f f(0.f);

    for(int i = 0; i < numBxDFs; i++)
    {
        if(bxdfs[i]->MatchesFlags(flags) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) || //if type includes reflection and reflective
                (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))    //if type includes transmission and not reflective
        {
            f += bxdfs[i]->f(wo, wi);
        }
    }

    return f;
}

// Use the input random number _xi_ to select
// one of our BxDFs that matches the _type_ flags.

// After selecting our random BxDF, rewrite the first uniform
// random number contained within _xi_ to another number within
// [0, 1) so that we don't bias the _wi_ sample generated from
// BxDF::Sample_f.

// Convert woW and wiW into tangent space and pass them to
// the chosen BxDF's Sample_f (along with pdf).
// Store the color returned by BxDF::Sample_f and convert
// the _wi_ obtained from this function back into world space.

// Iterate over all BxDFs that we DID NOT select above (so, all
// but the one sampled BxDF) and add their PDFs to the PDF we obtained
// from BxDF::Sample_f, then average them all together.

// Finally, iterate over all BxDFs and sum together the results of their
// f() for the chosen wo and wi, then return that sum.

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
    //PAGE 833

    //STEP 1
    // Use the input random number _xi_ to select
    // one of our BxDFs that matches the _type_ flags.

    //================================== choose which BxDF to sample ==================================
    int matchingComps = BxDFsMatchingFlags(type);

    if(matchingComps == 0)
    {
        *pdf = 0.0f;
        return Color3f(0.0f);
    }

    //================================== get BxDF pointer for chosen component ==================================
    std::vector<BxDF*> matchingBxDFs;

    for(int i = 0; i < numBxDFs; i++)
    {
        if(bxdfs[i]->MatchesFlags(type))
        {
            matchingBxDFs.push_back(bxdfs[i]);
        }
    }

    //================================== remap BxDF sample u to [0, 1) ==================================
    Point2f randomVal = xi;
    int randomIndex = std::min((int)glm::floor(randomVal[0] * matchingComps), (matchingComps - 1));//glm::ceil(randomVal[0] * (matchingBxDFs.size() - 1)); //randomIndex = randomVal[0] * numBxDFs; //THIS IS WRONG
    BxDF *_bxdf = matchingBxDFs[randomIndex];

    //STEP 2
    // After selecting our random BxDF, rewrite the first uniform random number contained
    // within _xi_ to another number within [0, 1) so that we don't bias the _wi_ sample generated from BxDF::Sample_f.
    randomVal[0] = glm::clamp(matchingComps * xi[0] - randomIndex, 0.0f, 1.0f);


    //STEP 3
    // Convert woW and wiW into tangent space and pass them to the chosen BxDF's Sample_f (along with pdf).
    // Store the color returned by BxDF::Sample_f and convert the _wi_ obtained from this function back into world space.

    //world to local
    Vector3f wi(0.0f);
    Vector3f wo = worldToTangent * woW;
    float newPdf = 0.0f;

    if(sampledType)
    {
        *sampledType = _bxdf->type;
    }

    //================================== Call to sample f for current bxdf ==================================
    Color3f f = _bxdf->Sample_f(wo, &wi, randomVal, &newPdf, sampledType);

    //CHECKING FOR 0 VALUE PDF
    if(newPdf == 0)
    {
        return Color3f(0.0f);
    }

    //local to world
    *wiW = tangentToWorld * wi;
    *pdf = newPdf;


    //================================== compute overall pdf with all matching BxDF's ==================================
    //STEP 4
    // Iterate over all BxDFs that we DID NOT select above (so, all but the one sampled BxDF)
    // and add their PDFs to the PDF we obtained from BxDF::Sample_f, then average them all together.

    if(!(_bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
    {
        for(int i = 0; i < numBxDFs; i++)
        {
            if(bxdfs[i] != _bxdf)// && bxdfs[i]->MatchesFlags(type))
            {
                *pdf += bxdfs[i]->Pdf(wo, wi);
            }
        }
    }

    //you want to average the pdf's no matter what type they are
    if(matchingComps > 1)
    {
        *pdf /= matchingComps;

    }


    //STEP 5
    // Finally, iterate over all BxDFs and sum together the results of their
    // f() for the chosen wo and wi, then return that sum.

    //================================== compute value of BSDF for sampled direction ==================================
    if(!(_bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
    {
        bool reflect = glm::dot(*wiW, normal) * glm::dot(woW, normal) > 0;
        f = Color3f(0.0f);

        for(int i = 0; i < numBxDFs; i++)
        {
            if(bxdfs[i]->MatchesFlags(type) &&
                    ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                     (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
            {
                f += bxdfs[i]->f(wo, wi);
            }

            //this is what i did for previous homework
//            if(bxdfs[i] != _bxdf)
//            {
//                f += bxdfs[i]->f(wo, wi);
//            }
        }
    }

    return f;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    //TODO

    //loop over the BxDFs and call their PDF() methods to find the PDF for an arbitrary sampled direction

    Vector3f wo = worldToTangent * woW;
    Vector3f wi = worldToTangent * wiW;

    if(wo.z == 0)
    {
        return 0.0f;
    }

    float pdf = 0.0f;
    int matchingComps = 0;

    for(int i = 0; i < numBxDFs; i++)
    {
        if(bxdfs[i]->MatchesFlags(flags))
        {
            pdf += bxdfs[i]->Pdf(wo, wi);
            matchingComps++;
        }//end if
    }//end for


    if(matchingComps > 0)
    {
        return pdf / matchingComps;
    }
    else
    {
        return 0.0f;
    }
}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    //TODO
    //PAGE 806 IN BOOK
    //cosine-sample the hemisphere, flipping the direction if necessary
    *wi = WarpFunctions::squareToHemisphereUniform(xi);//squareToHemisphereCosine(xi);
    if(wo.z < 0.0f)
    {
        wi->z *= -1.0f;
    }

    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
}
