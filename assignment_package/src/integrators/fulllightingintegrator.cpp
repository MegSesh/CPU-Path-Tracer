#include "fulllightingintegrator.h"

#include "directlightingintegrator.h"


Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundedEnergy) const
{
    //TODO

    Color3f finalOutputColor = Color3f(0.0f);
    Color3f throughput = Color3f(1.0f);

    int depthVal = recursionLimit;

    int numLights = scene.lights.size();
    if(numLights == 0)
    {
        return Color3f(0.0f);
    }

    Ray firstRay = ray;

    bool prevWasSpecular = false;

    while(depthVal > 0)
    {
        Ray currRay = firstRay;

        Intersection isect = Intersection();
        bool intersectionExists = scene.Intersect(firstRay, &isect);
        if(!intersectionExists)
        {
            finalOutputColor += Color3f(0.0f);
            break;
        }

        //index used to randomly select light
        int randLightIndex = std::min((int)glm::floor(sampler->Get1D() * numLights), (numLights - 1));


        // ============================================== SPECULARITY CHECK ==============================================
        /*
         * Since your path tracer computes the direct lighting a given intersection receives as its own term,
         * your path tracer must not include too much light. This means that every ray which already computed
         * the direct lighting term should not incorporate the Le term of the light transport equation into its light contribution.
         * In other words, unless a particular ray came directly from the camera or from a perfectly specular surface, Le should be ignored.
         */

        if(depthVal == recursionLimit || prevWasSpecular)
        {
            finalOutputColor += isect.Le(-currRay.direction) * throughput;
        }
        if(!isect.ProduceBSDF())
        {
            break;
        }



        // ============================================== TOTAL GLOBAL ILLUMINATION COMPONENT ==============================================
        /*
         * Separately from your direct lighting term, you should use an entirely new 2D uniform random variable to generate a new BSDF-based ωi using Sample_f.
         * You will multiply the color obtained from Sample_f with your ray throughput, along with the absolute-dot-product term and the 1/pdf term.

         * This effectively compounds the inherent material colors of all surfaces this ray has bounced from so far, so that when we incorporate
         * the lighting this particular ray bounce receives directly, it is attenuated by all of the surfaces our ray has previously hit.

         * To this end, once you have your updated throughput, multiply it with the direct lighting term
         * from above and add the result to your accumulated ray color.

         * Finally, make sure to update the ray that your while loop is using to originate from your
         * current intersection and travel in the direction of the ωi that you just computed.
         */

        Vector3f wi_GI = Vector3f(0.0f);
        float GI_pdf = 0.0f;
        BxDFType GI_type = BSDF_ALL;
        BxDFType GI_SampledType;

        Color3f GI_samplef_result = isect.bsdf->Sample_f(-currRay.direction, &wi_GI, sampler->Get2D(), &GI_pdf, GI_type, &GI_SampledType);
        float GI_absDot = AbsDot(wi_GI, isect.normalGeometric);

        Ray GISampleRay = isect.SpawnRay(wi_GI);
        firstRay = GISampleRay;




        // ============================================== TOTAL MULTIPLE IMPORTANCE SAMPLING ==============================================
        Color3f totalMISComponent = Color3f(0.0f);


        /* ======================= IF SPECULAR SURFACE =======================
         * Treat the direct lighting contribution of specular surfaces as black.
         * While the term we'd obtain from the BSDF-based sample would not actually be black,
         * we perform a slightly less costly BSDF contribution computation below,
         * so we're just going to ignore the direct lighting term entirely when it comes to specular materials.
         */

        prevWasSpecular = false;
        if(GI_SampledType & BSDF_SPECULAR)
        {
            totalMISComponent = Color3f(0.0f);
            prevWasSpecular = true;
        }
        else
        {
            // ========================== DIRECT LIGHTING ==========================

            Color3f DL_lightSample(0.0f);
            float DLWeight = 0.0f;
            Vector3f wi_DL = Vector3f(0.0f);
            float dl_pdf = 0.0f;
            BxDFType Dl_type = BSDF_ALL;

            Color3f dl_sampleLi_result = scene.lights.at(randLightIndex)->Sample_Li(isect, sampler->Get2D(), &wi_DL, &dl_pdf);

            if(dl_pdf == 0.0f)
            {
                DL_lightSample = Color3f(0.0f);
            }
            else
            {
                // ======= check for shadows =======
                Ray shadowRay = isect.SpawnRay(wi_DL);
                Intersection shadow_pt = Intersection();
                bool shadowIntersectionExists = scene.Intersect(shadowRay, &shadow_pt);
                Color3f vis = Color3f(0.0f);

                if(scene.lights.at(randLightIndex)->isPoint)    //if light you sampled is a point light
                {
                    if(isect.objectHit = nullptr)   //if you hit just the light
                    {
                        vis = Color3f(1.0f);
                    }
                    else if(!shadowIntersectionExists)  //if feeler ray intersection dne
                    {
                        vis = Color3f(1.0f);
                    }
                    else
                    {
                        float dist1 = glm::length(scene.lights.at(randLightIndex)->lightPos - isect.point);
                        float dist2 = glm::length(shadow_pt.point - isect.point);
                        vis = (dist2 < dist1 - 0.001) ? Color3f(0.0f) : Color3f(1.0f);
                    }
                }

                else if(scene.lights.at(randLightIndex)->isDistant && !shadowIntersectionExists)
                {
                    vis = scene.lights.at(randLightIndex)->L(shadow_pt, isect.normalGeometric);
                }

                else if(shadowIntersectionExists)
                {
                    vis = (shadow_pt.objectHit->areaLight != scene.lights.at(randLightIndex)) ? Color3f(0.0f) : Color3f(1.0f);
                }



                Color3f dl_f_result = isect.bsdf->f(-currRay.direction, wi_DL, Dl_type);
                float dl_absDot = AbsDot(wi_DL, isect.normalGeometric);

                // ======= Heuristics =======
                float pdf_wiDL_DL = dl_pdf;
                float pdf_wiDL_BSDF = isect.bsdf->Pdf(-currRay.direction, wi_DL, Dl_type);
                DLWeight = PowerHeuristic(1, pdf_wiDL_DL, 1, pdf_wiDL_BSDF);

                DL_lightSample = (dl_f_result * dl_sampleLi_result * dl_absDot * vis) / dl_pdf;
            }


            // ========================== BSDF ==========================
            Color3f DL_bsdfsample(0.0f);
            float BSDFWeight = 0.0f;
            Vector3f wi_BSDF = Vector3f(0.0f);
            float bsdf_pdf = 0.0f;
            BxDFType bsdf_SampledType;

            Color3f bsdf_samplef_result = isect.bsdf->Sample_f(-currRay.direction, &wi_BSDF, sampler->Get2D(), &bsdf_pdf, Dl_type, &bsdf_SampledType);
            float bsdf_absDot = AbsDot(wi_BSDF, isect.normalGeometric);

            if(bsdf_pdf == 0.0f)
            {
                DL_bsdfsample = Color3f(0.0f);
            }
            else
            {
                Ray bsdfSampleRay = isect.SpawnRay(wi_BSDF);
                Intersection bsdfIntersection = Intersection();
                bool bsdfIntersectionExists = scene.Intersect(bsdfSampleRay, &bsdfIntersection);

                if(bsdfIntersectionExists)
                {
                    if(bsdfIntersection.objectHit->areaLight == scene.lights.at(randLightIndex))
                    {
                        // ======= Heuristics =======
                        float pdf_wiBSDF_BSDF = bsdf_pdf;
                        float pdf_wiBSDF_DL = (scene.lights.at(randLightIndex)->Pdf_Li(isect, wi_BSDF));
                        BSDFWeight = PowerHeuristic(1, pdf_wiBSDF_BSDF, 1, pdf_wiBSDF_DL);

                        DL_bsdfsample = (bsdf_samplef_result * bsdf_absDot) / bsdf_pdf;
                    }
                    else
                    {
                        DL_bsdfsample = Color3f(0.0f);
                    }
                }//end if bsdf intersection exists
            }//end else pdf is not 0


            // ======================= CALCULATE TOTAL MULTIPLE IMPORTANCE SAMPLING COMPONENT =======================
            totalMISComponent += ((DL_lightSample * DLWeight) + (DL_bsdfsample * BSDFWeight)) * (float)numLights;
        }//end if not specular surface


        finalOutputColor += totalMISComponent * throughput;
        throughput *= ((GI_samplef_result * GI_absDot) / GI_pdf);    //make sure to update throughput after calculating final output color


        //======================= Russian roulette =======================
        float randRouletteNum = sampler->Get1D();
        float componentToCompare = glm::max(glm::max(throughput.x, throughput.y), throughput.z);

        if(depthVal <= recursionLimit - 3)
        {
            if(componentToCompare < randRouletteNum)
            {
                break;
            }
            else
            {
                throughput /= componentToCompare;
            }
        }//end russian roulette

        depthVal--;

        if(GI_pdf == 0.0f)      //check if global illumination pdf is 0
        {
            break;
        }

    }//end while loop

    return finalOutputColor;
}


/* ================== BALANCE AND POWER HEURISTICS ====================
 * At the bottom of directlightingintegrator.cpp, you will find definitions of two functions: BalanceHeuristic and PowerHeuristic.
 * Implement both of these functions so they can be used in your implementation of FullLightingIntegrator::Li.

 * PAGE 799 IN BOOK

 * For your final renders, we ask that you provide at least one example of a scene rendered with
 * the balance heuristic and one example of the same scene rendered with the power heuristic.
 * For best render results, we recommend you use the power heuristic for the rest of your images.
 */
float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    //if denom is 0, set to 0
    if(nf * fPdf == 0.0f && ng * gPdf == 0.0f)
    {
        return 0.0f;
    }
    return (nf * fPdf) / ((nf * fPdf) + (ng * gPdf));
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO

    float f = nf * fPdf;
    float g = ng * gPdf;

    //if denom is 0, set to 0
    if(f == 0.f && g == 0.f)
    {
        return 0.0f;
    }
    return (f * f) / ((f * f) + (g * g));
}

