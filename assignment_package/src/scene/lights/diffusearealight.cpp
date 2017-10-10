#include "diffusearealight.h"


std::shared_ptr<Shape> DiffuseAreaLight::getShape() const
{
    return shape;
}


/*
 * You will find this class under scene/lights/diffusearealight.h, and it represents an area light
 * that emits its energy equally in all directions from its surface.
 * For this class, you must implement one function, L,
 * which returns the light emitted from a point on the light's surface along a direction w, which is leaving the surface.
 * This function is invoked in Intersection::Le, which returns the light emitted by a point of intersection on a surface.
 *
 * PAGE 736 IN BOOK
 */

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    //TODO

    //isect.normal is the normal from the light
    //w is the ray leaving the surface

    if(!twoSided)
    {
        //check if w is in same direction as surface normal of isect
        //if you're looking at backside of light, return zero color
        //you're looking at backside if dot product is positive

        if(glm::dot(isect.normalGeometric, w) > 0.0f)
        {
            return emittedLight;
        }
        else
        {
            return Color3f(0.0f);
        }
    }


    return emittedLight;

}

/*
 * This function must perform four actions:

 * Get an Intersection on the surface of its Shape by invoking shape->Sample.
 * Check if the resultant PDF is zero or that the reference Intersection and the resultant
          Intersection are the same point in space, and return black if this is the case.
 * Set ωi to the normalized vector from the reference Intersection's point to the Shape's intersection point.
 * Return the light emitted along ωi from our intersection point.

 */
Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const
{
    Intersection resultantIntersection = shape->Sample(ref, xi, pdf);

    if(*pdf <= RayEpsilon)
    {
        return Color3f(0.0f);
    }
    if(resultantIntersection.point == ref.point)
    {
        return Color3f(0.0f);
    }

    *wi = glm::normalize(resultantIntersection.point - ref.point);

    return L(resultantIntersection, -*wi);
}


/*
 * Implement the Pdf_Li function declared in diffusearealight.h so that it returns the PDF of the
 * light's Shape given an Intersection from which the light is being viewed and a viewing direction wi.
 * PAGE 837 IN BOOK
 */
float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const
{
    //TODO

    Ray rayToLight = ref.SpawnRay(wi);
    Intersection isectLight;

    //if you don't intersect anything
    if(!shape->Intersect(rayToLight, &isectLight))
    {
        return 0.0f;
    }

    float absdot = AbsDot(isectLight.normalGeometric, glm::normalize(-wi));
    Vector3f radius = ref.point - isectLight.point;

    return (1.0f / area) * (glm::length2(radius) / absdot); //radius is from ref to new light intersection. NOT WI
}
