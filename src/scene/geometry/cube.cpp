#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

// auxiliary function: used to determine the normal vector of a cube at point p
glm::vec3 myGetCubeNormal(glm::vec3 pt)
{
    // front
    if(fequal(pt.z, 0.5f))
        return glm::vec3(0,0,1);
    // back
    if(fequal(pt.z, -0.5f))
        return glm::vec3(0,0,-1);
    // right
    if(fequal(pt.x, 0.5f))
        return glm::vec3(1,0,0);
    // left
    if(fequal(pt.x, -0.5f))
        return glm::vec3(-1,0,0);
    // top
    if(fequal(pt.y, 0.5f))
        return glm::vec3(0,1,0);
    // bottom
    if(fequal(pt.y, -0.5f))
        return glm::vec3(0,-1,0);

    // must be one of above situations.
    // default value: will NEVER return this if all calculations ARE CORRECT!
    return glm::vec3(1.0f);
}
Intersection Cube::GetIntersection(Ray r)
{
    Intersection intersection;

    // define xyz_min and xyz_max as -0.5 and 0.5
    float t_near = -INFINITY,t_far = INFINITY;
    const float xyz_min = -0.5f,xyz_max = 0.5f;

    bool miss = false;

    // process x, y and z components separately
    for (int i=0; i<3; i++)
    {
        if(fequal(r.direction[i], 0.0f)) // parallel
        {
            if(r.origin[i] <= xyz_min || r.origin[i] >= xyz_max)
            {
                miss = true;
                break;
            }
        }
        else  // not parallel
        {
            float t0 = (xyz_min - r.origin[i]) / r.direction[i];
            float t1 = (xyz_max - r.origin[i]) / r.direction[i];

            if(t0 > t1) {float temp=t0; t0=t1; t1=temp;}

            if(t0 > t_near) // update t_near
                t_near = t0;

            if(t1 < t_far) // update t_far
                t_far = t1;
        }
    }

    if(t_near > t_far)
        miss = true;

    if(!miss)
    {
        intersection.t = t_near;
        intersection.object_hit = this;
        intersection.point = r.origin + t_near*r.direction;
        intersection.normal = myGetCubeNormal(intersection.point);

        if(material != NULL)
        {
            intersection.color = material->base_color * Material::GetImageColor(GetUVCoordinates(intersection.point), material->texture);
        }

        SetNormalMappingValue(intersection);
    }

    return intersection;
}

void Cube::SamplingPointsOnSurface(const glm::vec3 hitPoint)
{
    SamplePoints.clear();
    SamplePoints.append(hitPoint);
}

void Cube::SetNormalMappingValue(Intersection &inter)
{
    if(material!=NULL && material->normal_map != NULL)
    {
        glm::vec3 N = glm::normalize(myGetCubeNormal(inter.point));
        glm::vec3 T;
        if(fequal(inter.point.y, 0.5f) || fequal(inter.point.y, -0.5f))
            T = glm::normalize(glm::cross(glm::vec3(1,0,0), N));
        else
            T = glm::normalize(glm::cross(glm::vec3(0,1,0), N));
        glm::vec3 B = glm::normalize(glm::cross(N, T));

        glm::mat4 NormalOrien(
                glm::vec4(T,0),
                glm::vec4(B,0),
                glm::vec4(N,0),
                glm::vec4(0,0,0,1));
        glm::vec3 normalMapValue = Material::GetImageColor(GetUVCoordinates(inter.point), material->normal_map);
        normalMapValue = (normalMapValue*2.0f - 1.0f);
        inter.normal = glm::normalize(glm::vec3(NormalOrien*glm::vec4(normalMapValue,0.0f)));
    }
    else
        inter.normal = myGetCubeNormal(inter.point);
}
//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    float u,v;
    if(fequal(point.x, 0.5f))//right
    {
        u = 0.25f - 0.25f * (point.z+0.5f);
        v = 0.33f - 0.33f * (point.y+0.5f);
    }
    else if(fequal(point.x, -0.5f))//left
    {
         u = 0.25f - 0.25f * (point.z+0.5f);
         v = 0.66f + 0.33f * (point.y+0.5f);
    }
    if(fequal(point.y, 0.5f))//top
    {
        u = 0.5f + 0.25f * (point.z+0.5f);
        v = 0.66f - 0.33f * (point.x+0.5f);
    }
    else if(fequal(point.y, -0.5f))//bottom
    {
        u = 0.25f - 0.25f * (point.z+0.5f);
        v = 0.66f - 0.33f * (point.x+0.5f);
    }
    if(fequal(point.z, 0.5f)) //front
    {
        u = 1.0f - 0.25f * (point.y+0.5f);
        v = 0.66f - 0.33f * (point.x+0.5f);
    }
    else if(fequal(point.z, -0.5f))//back
    {
        u = 0.25f + 0.25f * (point.y+0.5f);
        v = 0.66f - 0.33f * (point.x+0.5f);
    }

    return glm::vec2(u, v);
}


void Cube::setBBox()
{
    bBox = new BBox(glm::vec3(-0.51f), glm::vec3(0.51f));
    bBox->reComputeBoundingBox(transform.T());
    bBox->create();
    //bBox->geometryAttached = this;
}
