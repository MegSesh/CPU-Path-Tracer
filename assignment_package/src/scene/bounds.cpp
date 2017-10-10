#include "bounds.h"


/*
 * Intersect computes the intersection of the input ray with this bounding box.
 * Unlike the Intersect functions of the Shape classes, this does not output an entire Intersection data package,
 * but just outputs the t parameterization that the intersection would have with the ray.
 * This is useful for determining which bounding box is intersected first by a given ray.
 * Additionally, negative t values are valid if and only if the ray's origin lies within the bounding box.
 */
bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO

    float t_near = -1000000;
    float t_far = 1000000;

    for(int i = 0; i < 3; i++)
    {
        //check if ray is parallel to axis
        if(r.direction[i] == 0)
        {
            //you miss in this case
            if(r.origin[i] < min[i] || r.origin[i] > max[i])
            {
                return false;
            }//end if
        }//end if

        //if not parallel, do slab intersect check
        float t0 = (min[i] - r.origin[i]) / r.direction[i];
        float t1 = (max[i] - r.origin[i]) / r.direction[i];

        if(t0 > t1)
        {
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }//end if

        if(t0 > t_near)
        {
            t_near = t0;
        }//end if

        if(t1 < t_far)
        {
            t_far = t1;
        }//end if
    }//end for loop

    if(t_near < t_far)
    {
        float t_final = t_near > 0.0f ? t_near : t_far;

        //negative t values are valid if ray origin is inside bounding box
        if(t_final < 0.0f)
        {
            //if t is not in the bounding box, return false
            if(!((r.origin.x > min.x && r.origin.x < max.x) &&
                    (r.origin.y > min.y && r.origin.y < max.y) &&
                    (r.origin.z > min.z && r.origin.z < max.z)))
            {
                return false;
            }//end if
        }//end if

        *t = t_final;

    }//end if
    else
    {
        return false;
    }//end else

}//end intersect function


/*
 * Apply should transform the eight corners of the bounding box by the input Transform's matrix,
 * then compute a new axis-aligned bounding box that encompasses these corners
 * (which may no longer be axis aligned, especially if the Transform contained a rotation).
 * This function sets the invoking Bounds3f to be this bounding box, and returns the resultant bounds.
 * PAGE 95 IN BOOK
 */
Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO

    //compute 8 corners of bounding box and transform them
    Point3f frontUpperLeft = Point3f(tr.T() * glm::vec4(min.x, max.y, min.z, 1.0f));
    Point3f min_frontLowerLeft = Point3f(tr.T() * glm::vec4(min, 1.0f));
    Point3f frontUpperRight = Point3f(tr.T() * glm::vec4(max.x, max.y, min.z, 1.0f));
    Point3f frontLowerRight = Point3f(tr.T() * glm::vec4(max.x, min.y, min.z, 1.0f));

    Point3f backUpperLeft = Point3f(tr.T() * glm::vec4(min.x, max.y, max.z, 1.0f));
    Point3f backLowerLeft = Point3f(tr.T() * glm::vec4(min.x, min.y, max.z, 1.0f));
    Point3f max_backUpperRight = Point3f(tr.T() * glm::vec4(max, 1.0f));
    Point3f backLowerRight = Point3f(tr.T() * glm::vec4(max.x, min.y, max.z, 1.0f));

    //compute new bounding box
    Bounds3f output =
            Union(
                Union(
                    Union(
                        Union(
                            Union(
                                Union(
                                    Union(Bounds3f(min_frontLowerLeft), frontUpperLeft), frontUpperRight), frontLowerRight), backUpperLeft), backLowerLeft), max_backUpperRight), backLowerRight);

    this->min = output.min;
    this->max = output.max;

    return output;
}


/*
 * SurfaceArea, as its name implies, computes and returns the surface area of the invoking Bounds3f.
 * This will be used when you apply the surface area heuristic to choose a place at which to split yourBVH.
 */
float Bounds3f::SurfaceArea() const
{
    //TODO

    //Page 80 in book
    Vector3f diag = Diagonal();
    return 2.0f * ( (diag.x * diag.y)   +
                    (diag.x * diag.z)   +
                    (diag.y * diag.z));
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    return Union(b1, Point3f(p));
}
