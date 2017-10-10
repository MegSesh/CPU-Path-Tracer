#include "directlightingintegrator.h"

/*
 * You will write another Integrator class's Li function, this time so that it performs light source importance sampling
 * and evaluates the light energy that a given point receives directly from light sources.
 * That means that this Integrator will never recursively call Li.

 * Much of the code in this Integrator is the same as the code you wrote for NaiveIntegrator,
 * but rather than calling a BSDF's Sample_f to generate a ωi,
 * you will randomly select a light source from scene.lights and call its Sample_Li function.

 * Once you have done this, you can evaluate the remaining components of the Light Transport Equation (you already have Li).
 * Sample_Li invokes a few additional functions that you will have to implement; we will detail them below.

 * One final note: you must divide the PDF you obtain from Sample_Li by the number of light sources
 * in your scene to account for the fact that you are choosing a light at random uniformly from the set of all lights in the scene.
 * Mathematically, this just boosts the energy obtained from any one light by a factor of N, where N is the number of lights in the scene.

 */
Color3f DirectLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundedEnergy) const
{
    //TODO

    Intersection isect = Intersection();
    bool intersectionExists = scene.Intersect(ray, &isect);

    if(!intersectionExists)
    {
        return Color3f(0.0f);
    }

    Vector3f wi = Vector3f(0.0f);
    float pdf = 0.0f;
    BxDFType type = BSDF_ALL;

    Color3f le_result = isect.Le(-ray.direction);

    //========================= select random light source from scene.lights (same as bxdf - uniform) ==========================
    int lightsize = scene.lights.size();
    int randLightIndex = std::min((int)glm::floor(sampler->Get1D() * lightsize), (lightsize - 1));

    Color3f light_sampleLi_result = scene.lights.at(randLightIndex)->Sample_Li(isect, sampler->Get2D(), &wi, &pdf);

    if(!isect.ProduceBSDF())
    {
        return le_result;//return isect.Le(ray.direction);
    }

    //===================================== compute the rest of the LTE components ==============================

    //you use f because you just want the color
    Color3f f_result = isect.bsdf->f(-ray.direction, wi, type);

//    Color3f le_result = isect.Le(ray.direction);
    float absDot = AbsDot(wi, isect.normalGeometric);

    //add a check for lightsize being 0

    pdf /= (float)lightsize;


    //CHECK IF PDF IS 0
    if(pdf <= RayEpsilon)
    {
        return le_result;
    }

    //===================================== check for shadows and light visibility =====================================
    Ray newRay = isect.SpawnRay(wi);
    Intersection shadow_pt = Intersection();
    bool shadowIntersectionExists = scene.Intersect(newRay, &shadow_pt);

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

    return le_result + (f_result * light_sampleLi_result * absDot) * vis / pdf;

}






/* EXTRA NOTES
 * //    if(shadowIntersectionExists)
//    {
//        if(shadow_pt.objectHit->areaLight != scene.lights.at(randLightIndex))
//        {
//            return Color3f(0.0f);
//        }
//    }


    //if random light is a point light
        //check if object hit from light intersection is nullptr
            //make vis = 1 (you're assuming the ray hit the light and the light only)
        //find 2 distances,
        //from intersection point to light position and (lightpos - isect.point)

        //from intersection point that you spawned light from and the intersection that your light feeler gets
        //shadow feeler ray pos - isect.point

        //if distance to intersection < distance to light -.1 --> vis = 0
 */
