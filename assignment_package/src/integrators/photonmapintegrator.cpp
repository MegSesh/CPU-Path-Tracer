#include "photonmapintegrator.h"

#include "QTime"

Color3f PhotonMapIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundedEnergy) const
{
    //QTime time = QTime();
    //time.start();
    //,"roughness": 0.05

    Color3f finalOutputColor(0.0f);

    int numLights = scene.lights.size();
    if(numLights == 0)  return Color3f(0.0f);

    BxDFType type = BSDF_ALL;

    Intersection isect = Intersection();
    bool intersectionExists = scene.Intersect(ray, &isect);
    if(!intersectionExists) return Color3f(0.0f);
    if(!isect.ProduceBSDF())    return isect.Le(-ray.direction);//return Color3f(0.0f); //first time it hits light


    // ============================================== PICK ISECT AND RAY AND PHOTON CALCULATION ==============================================
    Ray currPhotonRay = ray;
    Intersection currPhotonIsect = isect;

    Vector3f photon_w_i = Vector3f(0.0f);
    float photon_pdf = 0.0f;
    //BxDFType photon_sampledType;

    Color3f totalPhotonComponent(0.0f);
    Color3f causticColor(0.0f);
    Color3f indirectColor(0.0f);

    while(true)
    {
        BxDFType photon_sampledType;
        Color3f photon_samplef_result = currPhotonIsect.bsdf->Sample_f(-currPhotonRay.direction, &photon_w_i, sampler->Get2D(), &photon_pdf, type, &photon_sampledType);

        if(photon_sampledType & BSDF_SPECULAR)
        {
            currPhotonRay = currPhotonIsect.SpawnRay(photon_w_i);
            Intersection newIsect = Intersection();
            bool newIsectExists = scene.Intersect(currPhotonRay, &newIsect);

            if(!newIsectExists)
            {
                return Color3f(0.0f);
            }

            if(!newIsect.ProduceBSDF())
            {
                return Color3f(0.0f);
            }

            currPhotonIsect = newIsect;
        }
        else
        {
            causticColor += photonMap.getColor(currPhotonIsect, 1.0f, 0);
            indirectColor += photonMap.getColor(currPhotonIsect, 1.0f, 1);
            //totalPhotonComponent = (causticColor + indirectColor) / 2.0f;
            break;
        }
    }//end while

    totalPhotonComponent += causticColor + indirectColor;





    // ============================================== TOTAL MULTIPLE IMPORTANCE SAMPLING ==============================================
    Color3f totalMISComponent = Color3f(0.0f);

    //index used to randomly select light
    int randLightIndex = std::min((int)glm::floor(sampler->Get1D() * numLights), (numLights - 1));


    // ========================== BSDF SAMPLING ==========================
    Color3f DL_bsdfsample(0.0f);
    float BSDFWeight = 0.0f;
    Vector3f wi_BSDF = Vector3f(0.0f);
    float bsdf_pdf = 0.0f;
    BxDFType mis_sampledType;


    Color3f bsdf_samplef_result = currPhotonIsect.bsdf->Sample_f(-currPhotonRay.direction, &wi_BSDF, sampler->Get2D(), &bsdf_pdf, type, &mis_sampledType);
    float bsdf_absDot = AbsDot(wi_BSDF, currPhotonIsect.normalGeometric);

    if(bsdf_pdf == 0.0f)    {DL_bsdfsample = Color3f(0.0f);}

    else
    {
        Ray bsdfSampleRay = currPhotonIsect.SpawnRay(wi_BSDF);
        Intersection bsdfIntersection = Intersection();
        bool bsdfIntersectionExists = scene.Intersect(bsdfSampleRay, &bsdfIntersection);

        if(bsdfIntersectionExists)
        {
            if(bsdfIntersection.objectHit->areaLight == scene.lights.at(randLightIndex))
            {
                // ======= Heuristics =======
                float pdf_wiBSDF_BSDF = bsdf_pdf;
                float pdf_wiBSDF_DL = (scene.lights.at(randLightIndex)->Pdf_Li(currPhotonIsect, wi_BSDF));
                BSDFWeight = PowerHeuristic(1, pdf_wiBSDF_BSDF, 1, pdf_wiBSDF_DL);

                DL_bsdfsample = (bsdf_samplef_result * bsdf_absDot) / bsdf_pdf;
            }

            else    {DL_bsdfsample = Color3f(0.0f);}
        }//end if bsdf intersection exists
    }//end else pdf is not 0


    // ========================== LIGHT SAMPLING ==========================
    Color3f DL_lightSample(0.0f);
    float DLWeight = 0.0f;
    Vector3f wi_DL = Vector3f(0.0f);
    float dl_pdf = 0.0f;
    Color3f dl_sampleLi_result = scene.lights.at(randLightIndex)->Sample_Li(currPhotonIsect, sampler->Get2D(), &wi_DL, &dl_pdf);

    if(dl_pdf == 0.0f)  {DL_lightSample = Color3f(0.0f);}

    else
    {
        // ======= check for shadows and light type and update visibility accordingly =======
        Ray shadowRay = currPhotonIsect.SpawnRay(wi_DL);
        Intersection shadow_pt = Intersection();
        bool shadowIntersectionExists = scene.Intersect(shadowRay, &shadow_pt);
        Color3f vis = Color3f(0.0f);

        //point light
        if(scene.lights.at(randLightIndex)->isPoint)
        {
            //if you hit just the light
            if(currPhotonIsect.objectHit = nullptr)   {vis = Color3f(1.0f);}

            //if feeler ray intersection DNE
            else if(!shadowIntersectionExists)  {vis = Color3f(1.0f);}

            else
            {
                float dist1 = glm::length(scene.lights.at(randLightIndex)->lightPos - currPhotonIsect.point);
                float dist2 = glm::length(shadow_pt.point - currPhotonIsect.point);
                vis = (dist2 < dist1 - 0.001) ? Color3f(0.0f) : Color3f(1.0f);
            }
        }

        //directional light
        else if(scene.lights.at(randLightIndex)->isDistant && !shadowIntersectionExists)
        {
            vis = scene.lights.at(randLightIndex)->L(shadow_pt, currPhotonIsect.normalGeometric);
        }

        //area light
        else if(shadowIntersectionExists)
        {
            vis = (shadow_pt.objectHit->areaLight != scene.lights.at(randLightIndex)) ? Color3f(0.0f) : Color3f(1.0f);
        }


        Color3f dl_f_result = currPhotonIsect.bsdf->f(-currPhotonRay.direction, wi_DL, type);
        float dl_absDot = AbsDot(wi_DL, currPhotonIsect.normalGeometric);

        // ======= Heuristics =======
        float pdf_wiDL_DL = dl_pdf;
        float pdf_wiDL_BSDF = currPhotonIsect.bsdf->Pdf(-currPhotonRay.direction, wi_DL, type);
        DLWeight = PowerHeuristic(1, pdf_wiDL_DL, 1, pdf_wiDL_BSDF);

        DL_lightSample = (dl_f_result * dl_sampleLi_result * dl_absDot * vis) / dl_pdf;
    }//end else pdf is not 0

    // ======================= CALCULATE TOTAL MULTIPLE IMPORTANCE SAMPLING COMPONENT =======================
    totalMISComponent += ((DL_lightSample * DLWeight) + (DL_bsdfsample * BSDFWeight)) * (float)numLights;


    finalOutputColor += totalMISComponent + totalPhotonComponent;

    //int timeSeconds = time.elapsed();
    //std::cout << "PHOTON MAP RENDER TIME: " << timeSeconds << std::endl;


    return finalOutputColor;
}//end Li function


float PhotonMapIntegrator::BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    if(nf * fPdf == 0.0f && ng * gPdf == 0.0f)  return 0.0f;
    return (nf * fPdf) / ((nf * fPdf) + (ng * gPdf));
}

float PhotonMapIntegrator::PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf) const
{
    float f = nf * fPdf;
    float g = ng * gPdf;
    if(f == 0.f && g == 0.f)    return 0.0f;
    return (f * f) / ((f * f) + (g * g));
}
