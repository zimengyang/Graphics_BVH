#pragma once

#include <scene/geometry/geometry.h>

//A sphere is assumed to have a radius of 0.5 and a center of <0,0,0>.
//These attributes can be altered by applying a transformation matrix to the sphere.
class Sphere : public Geometry
{
public:
    Intersection GetIntersection(Ray r);
    void create();
    glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    void SetNormalMappingValue(Intersection &inter);
    void SamplingPointsOnSurface(const glm::vec3 hitPoint);
    void setBBox();
    QString getGeometryName() {return QString("Sphere");}
};
