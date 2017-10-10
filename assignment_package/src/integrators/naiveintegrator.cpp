#include "naiveintegrator.h"


/*
 *  // Evaluate the energy transmitted along the ray back to
    // its origin, e.g. the camera or an intersection in the scene


 * You will implement the body of the NaiveIntegrator class's Li function,
 * which recursively evaluates the energy emitted from the scene along some ray back to the camera.
 * This function must find the intersection of the input ray with the scene and evaluate the result
 * of the light transport equation at the point of intersection.

 * Below is a list of functions you will find useful while implementing Li:

        Scene::Intersect
        Intersection::Le
        Intersection::ProduceBSDF
        Intersection::SpawnRay
        BSDF::Sample_f

 * Note that if Li is invoked with a depth value of 0 or if the intersection with the scene hits an object with no Material attached,
 * then Li should only evaluate and return the light emitted directly from the intersection.
 */

Color3f NaiveIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth, Color3f compoundedEnergy) const
{
    //TODO
    //return Color3f(0.f);


    /*

    if no intersection pt --> return le
    if depth == 0 or if object hit material dne --> return le

    else --> you have while loop going through depths
            recursively call li

            le + (f * li * absDot)/pdf

            V component only matters if you're making a point in the scene and shooting a ray to this point

            w_i is ray coming from intersection point

     wo = negative of ray direction

    //call Li again here recursively (depth --) and accumulate the result of this
    //call produceBSDF
    //create new ray
    //use this to call sample_f to fill in values you need for --> (f * li * absDot)/pdf

    //le + (f * li * absDot)/pdf
    //Lo(p, ωo) = Le(p, ωo) + ∫S f(p, ωo, ωi) Li(p, ωi) V(p', p) |dot(ωi, N)| dωi


    */

    Intersection *isect = new Intersection();
    bool intersectionExists = scene.Intersect(ray, isect);

    if(!intersectionExists)
    {
        return Color3f(0.0f);
    }

    Color3f li_result(0.0f);
    Vector3f wiW = Vector3f(0.0f);
    float pdf = 0.0f;
    BxDFType sampledType;
    BxDFType type = BSDF_ALL;


    Color3f le_result = isect->Le(-ray.direction);

    if(depth == 0 || !isect->ProduceBSDF())  //i used to have obj->material == nullptr and had call to produceBSDF after if check. but this is wrong. in producebsdf, it checks if you're hitting light already
    {
        return le_result;//return isect->Le(ray.direction);
    }


    //(const Vector3f &woW, Vector3f *wiW, const Point2f &xi, float *pdf, BxDFType type, BxDFType *sampledType)
    Color3f samplef_result = isect->bsdf->Sample_f(-ray.direction, &wiW, sampler->Get2D(), &pdf, type, &sampledType);

    //Color3f le_result = isect->Le(-ray.direction);

    float absDot = glm::abs(glm::dot(wiW, isect->normalGeometric));

    Ray newRay = isect->SpawnRay(wiW);
    li_result = Li(newRay, scene, sampler, depth - 1, Color3f(0.0f));

    if(pdf <= RayEpsilon)
    {
        return le_result;//Color3f(0.0f);
    }

    return le_result + (samplef_result * li_result * absDot) / pdf;

}
