#pragma once

#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/materials/material.h>
#include <scene/transform.h>
#include <scene/geometry/bbox.h>

class Intersection;//Forward declaration because Intersection and Geometry cross-include each other
class Material;
class BBox;

//Geometry is an abstract class since it contains a pure virtual function (i.e. a virtual function that is set to 0)
class Geometry : public Drawable
{
public:
    Geometry():
    name("GEOMETRY"), transform()
    {
        material = NULL;
        bBox = NULL;
    //    SamplePoints.clear();
    }

    virtual ~Geometry(){ delete bBox;}
    virtual Intersection GetIntersection(Ray r) = 0;
    virtual void SetMaterial(Material* m){material = m;}
    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point) = 0;
    virtual void SetNormalMappingValue(Intersection& inter) = 0;
    virtual QString getGeometryName() = 0;

    QString name;//Mainly used for debugging purposes
    Transform transform;
    Material* material;

    QList<glm::vec3> SamplePoints;
    virtual void SamplingPointsOnSurface(const glm::vec3 hitPoint) = 0;

    // box bounding pointer
    BBox *bBox;
    // set bounding box
    virtual void setBBox() = 0;

};
