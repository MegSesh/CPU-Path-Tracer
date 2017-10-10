#include "cube.h"
#include <iostream>

Point3f Cube::sampleSurface(std::shared_ptr<Sampler> sampler, Vector3f* t, Vector3f* b, Normal3f* n) const
{
    return Point3f(0.0f);
}

Bounds3f Cube::WorldBound() const
{
    //TODO
    Point3f min(-0.5f, -0.5f, -0.5f);
    Point3f max(0.5f, 0.5f, 0.5f);
    Bounds3f objBounds = Bounds3f(min, max);
    return objBounds.Apply(transform);
}


Intersection Cube::Sample(const Point2f &xi, Float *pdf) const
{
    return Intersection();
}


float Cube::Area() const
{
    //TODO
    return 0;
}

// Returns +/- [0, 2]
int GetFaceIndex(const Point3f& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i * glm::sign(P[i]);
            val = glm::abs(P[i]);
        }
    }
    return idx;
}


Normal3f GetCubeNormal(const Point3f& P)
{
    int idx = glm::abs(GetFaceIndex(Point3f(P)));
    Normal3f N(0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}


bool Cube::Intersect(const Ray& r, Intersection* isect) const
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }
    if(t_n < t_f)
    {
        float t = t_n > 0 ? t_n : t_f;
        if(t < 0.0f)
        {
            return false;
        }

        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t*r_loc.direction, 1);
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}


void Cube::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    *nor = glm::normalize(transform.invTransT() * GetCubeNormal(P));
    //TODO: Compute tangent and bitangent

    /*
     * For every ComputeTBN, you must add code that computes the world-space tangent and bitangent.
     * Note that unlike transforming a surface normal, transforming a tangent vector from
            object space into world space does not use the inverse transpose of the object's model matrix,
            but rather just its model matrix. If you consider transforming any plane from one space to another space,
            you can see that any vector within the plane (i.e. a tangent vector) must be transformed by the matrix used to transform the whole plane.


     * You might be wondering why these additional vectors are needed in the first place;
     * it's because we'll need them to create matrices that transform the rays ωo and ωi
     * from world space into tangent space and back when evaluating BSDFs.
     * By tangent space, we mean a coordinate system in which the surface normal at the point of intersection is treated as (0, 0, 1)
     * and a tangent and bitangent at the intersection are treated as (1, 0, 0) and (0, 1, 0) respectively.
     * These matrices are analogous to the orientation component of a camera's View matrix and its inverse.
     *
     * normal = (001)
     * tangent = (100)
     * bitangent = (010)
     */

//    *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(1.0, 0.0, 0.0, 0.0)));
//    *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 1.0, 0.0, 0.0)));

    Normal3f _normal = GetCubeNormal(P);

    //positive x
    if(_normal == Normal3f(1.0, 0.0, 0.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 0.0, -1.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, -1.0, 0.0, 0.0)));
    }
    //negative x
    else if(_normal == Normal3f(-1.0, 0.0, 0.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 0.0, 1.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, -1.0, 0.0, 0.0)));
    }
    //positive y
    else if(_normal == Normal3f(0.0, 1.0, 0.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(1.0, 0.0, 0.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 0.0, 1.0, 0.0)));
    }
    //negative y
    else if(_normal == Normal3f(0.0, -1.0, 0.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(1.0, 0.0, 0.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 0.0, -1.0, 0.0)));
    }
    //positive z
    else if(_normal == Normal3f(0.0, 0.0, 1.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(1.0, 0.0, 0.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, -1.0, 0.0, 0.0)));
    }
    //negative z
    else if(_normal == Normal3f(0.0, 0.0, -1.0))
    {
        *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(-1.0, 0.0, 0.0, 0.0)));
        *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, -1.0, 0.0, 0.0)));
    }
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}
