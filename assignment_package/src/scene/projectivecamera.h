#pragma once

#include <scene/camera.h>
#include <warpfunctions.h>
#include <samplers/sampler.h>

//374 in book
class ProjectiveCamera : public Camera
{
public:
    ProjectiveCamera()
        : Camera(), lensRadius(0.0f), focalDistance(1.0f)
    {}

    ProjectiveCamera(const Camera &c, const Float lensr, const Float focald)
        : Camera(c), lensRadius(lensr), focalDistance(focald)
    {}

    virtual Ray RaycastNDC(float ndc_x, float ndc_y) const; //Creates a ray in 3D space given a 2D point in normalized device coordinates.

private:
    Float lensRadius;       //how thick your lens is
    Float focalDistance;    //how sharply you see things
};

