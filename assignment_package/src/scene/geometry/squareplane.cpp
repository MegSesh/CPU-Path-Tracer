#include "squareplane.h"


Point3f SquarePlane::sampleSurface(std::shared_ptr<Sampler> sampler, Vector3f *t, Vector3f *b, Normal3f *n) const
{
    Point2f pt = sampler->Get2D();
    ComputeTBN(glm::vec3(pt, 0.0), n, t, b);
    Point3f origin = glm::vec3(transform.T() * glm::vec4(pt, 0.0f, 1.0f));
    return origin;
}

Bounds3f SquarePlane::WorldBound() const
{
    Point3f min(-0.5f, -0.5f, 0.0f);
    Point3f max(0.5f, 0.5f, 0.0f);
    Bounds3f objBounds = Bounds3f(min, max);
    return objBounds.Apply(transform);
}


float SquarePlane::Area() const
{
    //TODO
    Vector3f scale = transform.getScale();
    return scale.x * scale.y;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}


/*
 * Generate a world-space point on the surface of the shape.
 * Create an Intersection to return.
 * Set the point and normal of this Intersection to the correct values.
 * Set the output PDF to the correct value, which would be a uniform PDF with respect to surface area.
 */
Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const
{
    //return Intersection();
    //DO NOT USE t3. use transform.t

    //square plane is centered at the origin, so offset it so you can sample the entire thing (other you'll only sample top quadrant)

    glm::vec4 worldspace_pt(xi.x - 0.5f, xi.y - 0.5f, 0.0f, 1.0f);

    worldspace_pt = transform.T() * worldspace_pt;

    Intersection output = Intersection();
    output.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0.0f, 0.0f, 1.0f));
    output.point = Point3f(worldspace_pt);
    *pdf = 1.0f / Area();

    return output;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Normal3f(0,0,1));
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

    *tan = glm::normalize(Vector3f(transform.T() * glm::vec4(1.0, 0.0, 0.0, 0.0)));
    *bit = glm::normalize(Vector3f(transform.T() * glm::vec4(0.0, 1.0, 0.0, 0.0)));
    //*bit = glm::normalize(Vector3f(transform.T() * glm::vec4(glm::cross(*nor, *tan), 0.0f)));
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}
