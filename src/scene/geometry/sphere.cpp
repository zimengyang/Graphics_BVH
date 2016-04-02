#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;

Intersection Sphere::GetIntersection(Ray r)
{
    Intersection intersection; // store result intersection

    // substitute ray function to sphere equation to get a quadratic equation
    float A = glm::length(r.direction) * glm::length(r.direction);  // A = ||direction||^2
    float B = 2 * glm::dot(r.direction, r.origin);   // B = 2*direction*origin
    float C = glm::length(r.origin) * glm::length(r.origin) - 0.5f * 0.5f;  // C = ||origin||^2 - r^2

    float disc = B*B - 4*A*C; // calculate discriminant
    if(disc > 0 || fequal(disc, 0.0f)) // disc>=0 : exist intersection
    {
        float t0 = (-B - sqrt(disc))/(2*A);
        float t1 = (-B + sqrt(disc))/(2*A);

        if(t0 > 0)  // t0 will be intersection, done.
        {
            intersection.t = t0;
        }
        else
        {
            if(t1 > 0) // t0 < 0, t1 > 0, t1 will be intersection point
            {
                intersection.t = t1;
            }
        }
    }

    // t > 0: can get a valid solution of intersection, set the other properties of intersection
    if(intersection.t > 0)
    {
        intersection.object_hit = this;
        intersection.point = r.origin + intersection.t * r.direction;
        intersection.normal = glm::normalize(intersection.point);
        if(material != NULL)
        {
            intersection.color = material->base_color * Material::GetImageColor(GetUVCoordinates(intersection.point), material->texture);
        }

        SetNormalMappingValue(intersection);

    }

    // if no valid solution was found, intersection will returned as the default constructor value (t = -1, object_hit = NULL)
    return intersection;
}

void Sphere::setBBox()
{

    bBox = new BBox(glm::vec3(-0.5f), glm::vec3(0.5f));
    bBox->reComputeBoundingBox(transform.T());
    bBox->create();
   // bBox->geometryAttached = this;
}

void Sphere::SamplingPointsOnSurface(const glm::vec3 hitPoint)
{
    SamplePoints.clear();


    float theta;
    float phi;
    float r=0.5f;
    float delta = PI/7.0f;
    float range = 0*delta;

    float Phi = asin(hitPoint.z);
    float Theta = atan2(hitPoint.y,hitPoint.x);

   // std::cout<<"start sampling:"<<std::endl;
    for (phi=Phi-range;phi<=Phi+range;phi+=delta)
        for(theta=Theta-range;theta<=Theta+range;theta+=delta)
        {
            glm::vec3 point(r*cos(phi)*cos(theta),r*cos(phi)*sin(theta),r*sin(phi));
            glm::vec4 temp(transform.T() * glm::vec4(point, 1.0f));
            glm::vec3 result(glm::vec3(temp.x/temp.w,temp.y/temp.w,temp.z/temp.w));
            SamplePoints.push_back(result);

        //    std::cout<<"point = "<<result.x<<","<<result.y<<","<<result.z<<'\n';
        }
   // std::cout<<"end sampling\n"<<std::endl;
}

void Sphere::SetNormalMappingValue(Intersection& inter)
{
    if(material != NULL && material->normal_map != NULL)
    {
    glm::vec3 N = glm::normalize(inter.normal);
    glm::vec3 T = glm::normalize(glm::cross(glm::vec3(0,1,0), N));
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
         inter.normal = glm::normalize(inter.normal);

}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}


void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}


void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}

glm::vec2 Sphere::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec3 d = glm::normalize(point);
    float u = 0.5f + atan2(d.x, d.z) / (2.0f * PI);
    float v = asin(d.y) / PI + 0.5f;
    return glm::vec2(u, v);
}
