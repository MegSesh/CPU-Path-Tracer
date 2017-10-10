#include "projectivecamera.h"


Ray ProjectiveCamera::RaycastNDC(float ndc_x, float ndc_y) const
{
    //Note: doing this in camera space

    glm::vec3 P = ndc_x*H + ndc_y*V;
    Ray result(Point3f(0.0f), glm::normalize(P));

    std::shared_ptr<Sampler> sampler;

    if(lensRadius > 0.0f)
    {
        Point3f pLens = lensRadius * WarpFunctions::squareToDiskConcentric(sampler->Get2D());
        Float ft = focalDistance / result.direction.z;
        Point3f pFocus = result.origin * ft + result.direction;


        //don't multiply with viewproj, just add eye to origin. direction will be the same, but don't multiply viewproj
        //result.origin = eye + pLens;
        //result.direction = glm::normalize(pFocus - result.origin);

        result.origin = Point3f(GetViewProj() * glm::vec4(pLens, 0.0f));
        result.direction = Point3f(GetViewProj() * glm::vec4(glm::normalize(pFocus - result.origin), 0.0f));
    }
    else
    {
        result.origin = Point3f(GetViewProj() * glm::vec4(result.origin, 0.0f));
        result.direction = Point3f(GetViewProj() * glm::vec4(result.direction, 0.0f));
    }

    return result;
}
