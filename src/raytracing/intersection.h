#pragma once
#include <la.h>
#include <scene/geometry/geometry.h>
#include <raytracing/ray.h>
#include <scene/scene.h>
#include <scene/geometry/bbox.h>
#include <iostream>

class Material;
class Geometry;
class Scene;
class BVHNode;

class Intersection
{
public:
    Intersection();

    glm::vec3 point;      //The place at which the intersection occurred
    glm::vec3 normal;     //The surface normal at the point of intersection
    float t;              //The parameterization for the ray (in world space) that generated this intersection.
                          //t is equal to the distance from the point of intersection to the ray's origin if the ray's direction is normalized.
    Geometry* object_hit; //The object that the ray intersected, or NULL if the ray hit nothing.

    glm::vec3 color;      //The color on the surface of the object hit
};

class IntersectionEngine
{
public:
    IntersectionEngine();
    Scene *scene;

    BVHNode *root;
    Intersection GetIntersection(const Ray &r);
};

class BVHNode
{
public:
    BVHNode();

    BBox bBox;
    BVHNode *left;
    BVHNode *right;

    int depth;
    QList<Geometry*> geometryAttached;//sorted!!

    void createBVH(int dep);

    static bool compareNodeX(Geometry* &a, Geometry* &b);
    static bool compareNodeY(Geometry* &a, Geometry* &b);
    static bool compareNodeZ(Geometry* &a, Geometry* &b);

    static BVHNode* buildBVHTree(QList<Geometry*> objects);
    static void releaseTree(BVHNode* root);

    Intersection getIntersection(const Ray &r);

    ~BVHNode(){}
};

//debug
void PrintIntersectionInfo(const Intersection &inter);
#define RAY_TRACING
