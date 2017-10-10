#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented uniform disk warping!");

    Point3f output(0.0f);
    float r = std::sqrt(sample[0]);
    float theta = 2.0f * Pi * sample[1];

    float x = r * cos(theta);
    float y = r * sin(theta);
    //float z = std::sqrt(1.0f - (x * x) - (y * y));    //ITS A DISK, SO Z IS 0

    output = Point3f(x, y, 0.0f);
    return output;
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented concentric disk warping!");

    Point3f output(0.0f);
    Point2f uOffset = 2.0f * sample - Point2f(1.0f, 1.0f);

    if (uOffset.x == 0.0f + RayEpsilon && uOffset.y == 0.0f + RayEpsilon)
    {
       return Point3f(0.0f);
    }

    float theta = 0.0f;
    float r = 0.0f;

    if(std::fabs(uOffset.x) > std::fabs(uOffset.y))
    {
        r = uOffset.x;
        theta = (Pi / 4.0f) *  (uOffset.y / uOffset.x);
    }

    else
    {
        r = uOffset.y;
        theta = (Pi / 2.0f) - (Pi / 4.0f) * (uOffset.x / uOffset.y);
    }

    output = r * Point3f(cos(theta), sin(theta), 0.0f);
    return output;
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    //TODO
    //return 0;
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented uniform sphere warping!");

    Point3f output(0.0f);

    float z = 1.0f - 2.0f * sample[0];
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2.0f * Pi * sample[1];
    output = Point3f(r * cos(phi), r * sin(phi), z);

    return output;
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    //TODO
    //return 0;

    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented sphere cap warping!");

    Point3f output(0.0f);

    float phi = 2.0f * Pi * sample[1];

    float converted = ((thetaMin * Pi) / 180);

    float z = 1.0f - (  ((2.0f * (Pi - converted)) / Pi)    *   sample[0]    );
    float x = cos(phi) * std::sqrt(1.0f - (z * z));//std::max(thetaMin, 1.0f - z * z));
    float y = sin(phi) * std::sqrt(1.0f - (z * z));//std::max(thetaMin, 1.0f - z * z));

    output = Point3f(x, y, z);

    return output;
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    //TODO
    //return 0;

    float converted = ((180.0f - thetaMin) * Pi) / 180.0f;
    return 1.0f / (2.0f * Pi * (1.0f - cos(converted)));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented uniform hemisphere warping!");

    Point3f output(0.0f);
    float z = sample[0];
    float r = std::sqrt(std::max(0.0f, 1.0f - z * z));
    float phi = 2.0f * Pi * sample[1];
    output = Point3f(r * cos(phi), r * sin(phi), z);
    return output;
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    //TODO
    //return 0;

    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    //TODO
    //throw std::runtime_error("You haven't yet implemented cosine-weixhted hemisphere warping!");

    Point3f output(0.0f);
    Point3f d = squareToDiskConcentric(sample);
    float z = std::sqrt(std::max(0.0f, 1.0f - (d[0] * d[0]) - (d[1] * d[1])));
    output = Point3f(d.x, d.y, z);
    return output;
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    //TODO
    //return 0;

    return sample[2] * InvPi;
}
