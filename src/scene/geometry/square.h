#pragma once
#include <scene/geometry/geometry.h>

//A square is aligned with the XY plane with a normal aligned with the positive Z axis. Its side length is 1, and it is centered at the origin.
//These attributes can be altered by applying a transformation matrix to the square.
class SquarePlane : public Geometry
{
    Intersection GetIntersection(Ray r);
    void create();
    glm::vec2 GetUVCoordinates(const glm::vec3 &point);
    void SetNormalMappingValue(Intersection& inter);
    void SamplingPointsOnSurface(const glm::vec3 hitPoint);
    void setBBox();
    QString getGeometryName() {return QString("Square");}
};

