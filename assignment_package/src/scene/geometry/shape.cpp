#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


//Point3f Shape::sampleSurface(std::shared_ptr<Sampler> sampler, Vector3f* t, Vector3f* b, Normal3f* n) const
//{
//    return Point3f(0.0f);
//}


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}



float Shape::Pdf(const Intersection &ref, const Vector3f &wi) const
{
    return 0.0f;
}

/*
 * The Shape class actually declares two different Sample functions,
 * but the one you must implement in the base Shape class is the one
 * that takes in three inputs rather than two.

 * This function must invoke the two-input Sample, to be implemented by subclasses,
 * and generate a ωi from the resulting Intersection.

 * From there, it must convert the PDF obtained from the other
 * Sample function from a PDF with respect to surface area to a
 * PDF with respect to solid angle at the reference point of intersection.

 * As always, be careful of dividing by zero! Should this occur, you should set your PDF to zero.
 *
 * BOOK PAGE 838
 */
Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    //return Intersection();

    Intersection output = Sample(xi, pdf);

    //generate a wi from resulting Intersection
    Vector3f wi = output.point - ref.point;

    float absdot = AbsDot(output.normalGeometric, glm::normalize(-wi));

    if(absdot == 0.0f)
    {
        *pdf = 0.0f;
    }
    else
    {
        *pdf *= (glm::length2(wi) / absdot);
    }

    return output;
}
