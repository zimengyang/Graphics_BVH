#include <raytracing/intersection.h>
#include "scene/geometry/mesh.h"

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    t(-1),
    color(glm::vec3(0))
{
    object_hit = NULL;
}

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
    root = NULL;
}

// using bvh to get intersection
Intersection IntersectionEngine::GetIntersection(const Ray &r)
{
    Intersection inter = root->getIntersection(r);
    if(inter.t > 0)
    {
        return inter;
    }
    else
    {
        return Intersection();
    }
}

// normal intersection test
/*
Intersection IntersectionEngine::GetIntersection(const Ray &r)
{
    Intersection intersection;
    float t_min = INFINITY;

    //get all objects in scene
    for(Geometry *i : scene->objects)
    {
        Ray transformedRay = r.GetTransformedCopy(i->transform.invT());  // transformed new ray copy
        Intersection temp =  i->GetIntersection(transformedRay);

        if(temp.t > 0)
        {
            // transform point in Intersection to world coordinate using object->transform_matrix
            glm::vec4 trans_point = i->transform.T() * glm::vec4(temp.point, 1.0f);
            temp.point = glm::vec3(trans_point.x / trans_point.w, trans_point.y / trans_point.w, trans_point.z / trans_point.w);

            //multiply normal by inverse transpose of the model matrix to correctly bring it into world space
            glm::vec4 trans_normal = i->transform.invTransT() * glm::vec4(temp.normal, 0.0f);
            temp.normal = glm::normalize(glm::vec3(trans_normal.x,trans_normal.y,trans_normal.z));

            // update t by calculate the distance between intersection point and eye
            temp.t = glm::length(temp.point - r.origin);

            #ifdef  RAY_TRACING
            if(temp.t < t_min){ // closer to camera
                t_min = temp.t;
                intersection = temp;
            }
            #else
            if(temp.t >= scene->camera.near_clip && temp.t <= scene->camera.far_clip) // the intersection is within the clip planes
            {
                if(temp.t < t_min){ // closer to camera
                t_min = temp.t;
                intersection = temp;
                }
            }
            #endif
        }
    }

    if(intersection.object_hit != NULL)
    {
        return intersection;
    }
    else
      return Intersection(); // default-constructed Intersection
}
*/


//
void PrintIntersectionInfo(const Intersection & inter)
{
    std::cout<<"hit point = "
              <<inter.point.x<<","<<inter.point.y<<","<<inter.point.z<<std::endl;
    std::cout<<"hit object name = "
              <<inter.object_hit->name.toStdString()<<std::endl;
    std::cout<<" t= "
               <<inter.t<<std::endl;
    std::cout<<"color = "
            <<inter.color.x<<","<<inter.color.y<<","<<inter.color.z<<std::endl;
}

BVHNode::BVHNode()
{
    left = NULL;
    right = NULL;
    depth = 0;
    geometryAttached.clear();
}

BVHNode* BVHNode::buildBVHTree(QList<Geometry *> objects)
{
    BVHNode* root = new BVHNode();
    root->depth = 0;
    for(Geometry* g : objects)
    {
        root->geometryAttached.push_back(g);
        root->bBox.minBounding = BBox::mMinVector(g->bBox->minBounding, root->bBox.minBounding);
        root->bBox.maxBounding = BBox::mMaxVector(g->bBox->maxBounding, root->bBox.maxBounding);
    }
    root->createBVH(0);
    return root;

}

void BVHNode::createBVH(int dep)
{
    depth = dep;
    if(geometryAttached.size() == 1) // leaf node
    {
        bBox = BBox(*geometryAttached[0]->bBox);
        bBox.create();
        return;
    }
    else
    {
        int compareIndex = depth % 3;

        switch (compareIndex)
        {
        case 0:
            qSort(geometryAttached.begin(), geometryAttached.end(), BVHNode::compareNodeX);
            break;
        case 1:
            qSort(geometryAttached.begin(), geometryAttached.end(), BVHNode::compareNodeY);
            break;
        case 2:
            qSort(geometryAttached.begin(), geometryAttached.end(), BVHNode::compareNodeZ);
            break;
        }

        int middle = geometryAttached.size() / 2;
        if(middle != 0) //have left tree
        {
            left = new BVHNode();
            for(int i=0;i<middle;i++)
            {
                Geometry* tmp = geometryAttached[i];
                left->geometryAttached.push_back(tmp);
                left->bBox.minBounding = BBox::mMinVector(left->bBox.minBounding, tmp->bBox->minBounding);
                left->bBox.maxBounding = BBox::mMaxVector(left->bBox.maxBounding, tmp->bBox->maxBounding);
            }
            left->createBVH(dep + 1);
        }

        right = new BVHNode();
        for(int i=middle;i<geometryAttached.size();i++)
        {
            Geometry* tmp = geometryAttached[i];
            right->geometryAttached.push_back(tmp);
            right->bBox.minBounding = BBox::mMinVector(right->bBox.minBounding, tmp->bBox->minBounding);
            right->bBox.maxBounding = BBox::mMaxVector(right->bBox.maxBounding, tmp->bBox->maxBounding);
        }
        right->createBVH(dep + 1);
    }

    bBox = BBox(bBox.minBounding, bBox.maxBounding);
    bBox.create();
}

void BVHNode::releaseTree(BVHNode *root)
{
    if(root == NULL)
        return;

    if(root->left != NULL)
        releaseTree(root->left);
    if(root->right != NULL)
        releaseTree(root->right);

    //std::cout<<"release BVH node: depth = "<<root->depth<<", objects = "<<root->geometryAttached.size()<<std::endl;
    delete root;
    root = NULL;
}

Intersection BVHNode::getIntersection(const Ray &r)
{
    if(geometryAttached.size() < 1)
        return Intersection();

    Intersection intersection = bBox.getIntersection(r);
    if(intersection.t <= 0)
        return Intersection();

    if(left == NULL && right == NULL) // leaf
    {
        Geometry* g = geometryAttached[0];
        Ray transformedRay(r.GetTransformedCopy(g->transform.invT()));
        Intersection inter = g->GetIntersection(transformedRay);
        if(inter.t > 0)
        {
            // transform point in Intersection to world coordinate using object->transform_matrix
            glm::vec4 trans_point = g->transform.T() * glm::vec4(inter.point, 1.0f);
            inter.point = glm::vec3(trans_point.x / trans_point.w, trans_point.y / trans_point.w, trans_point.z / trans_point.w);

            //multiply normal by inverse transpose of the model matrix to correctly bring it into world space
            glm::vec4 trans_normal = g->transform.invTransT() * glm::vec4(inter.normal, 0.0f);
            inter.normal = glm::normalize(glm::vec3(trans_normal.x,trans_normal.y,trans_normal.z));

            // update t by calculate the distance between intersection point and eye
            inter.t = glm::length(inter.point - r.origin);
            return inter;
        }
        else
            return Intersection();
    }

    Intersection leftInter, rightInter;
    if(left != NULL)
        leftInter = left->getIntersection(r);
    if(right != NULL)
        rightInter = right->getIntersection(r);

    if(leftInter.t > 0 && rightInter.t > 0)
    {
        if(leftInter.t < rightInter.t)
            return leftInter;
        else
            return rightInter;
    }

    if(leftInter.t <= 0 && rightInter.t > 0)
        return rightInter;

    if(leftInter.t > 0 && rightInter.t <= 0)
        return leftInter;

    if(leftInter.t <=0 && rightInter.t <= 0)
        return Intersection();
}


bool BVHNode::compareNodeX(Geometry *&a, Geometry *&b)
{
    return a->bBox->maxBounding[0] < b->bBox->minBounding[0];
}

bool BVHNode::compareNodeY(Geometry *&a, Geometry *&b)
{
    return a->bBox->maxBounding[1] < b->bBox->minBounding[1];
}
bool BVHNode::compareNodeZ(Geometry *&a, Geometry *&b)
{
    return a->bBox->maxBounding[2] < b->bBox->minBounding[2];
}
