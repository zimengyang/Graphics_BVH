#include <scene/geometry/square.h>

Intersection SquarePlane::GetIntersection(Ray r)
{
    Intersection intersection;

    // randomly choose a point S on plane
    srand(time(0));
    float sx = float(rand()/RAND_MAX) - 0.5f;
    float sy = float(rand()/RAND_MAX) - 0.5f;

    glm::vec3 S(sx,sy,0.0f); // point S on plane (x,y,0)
    glm::vec3 N(0,0,1);      // normal vector of plane

    // the ray is not parallel to surface, dot(N,r.direction) = 0 means ray is parallel to surface
    if(!fequal(glm::dot(N, r.direction), 0.0f))
    {
        float t = glm::dot(N, S-r.origin) / glm::dot(N, r.direction);
        if(t <= 0.0f)
            return Intersection();

        glm::vec3 P = r.origin + t*r.direction;

        // hit point p is inside square plane
        if(P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
        {
            intersection.t = t;
            intersection.point = P;
            intersection.normal = N;
            intersection.object_hit = this;
            if(material != NULL)
            {
                intersection.color = material->base_color * Material::GetImageColor(GetUVCoordinates(intersection.point), material->texture);
            }

            SetNormalMappingValue(intersection);
        }
    }

    return intersection;
}

void SquarePlane::setBBox()
{
    bBox = new BBox(glm::vec3(-0.51f, -0.51f, 0.0f), glm::vec3(0.51f, 0.51f, 0.0f));
    bBox->reComputeBoundingBox(transform.T());
    bBox->create();
    //bBox->geometryAttached = this;
}

void SquarePlane::SamplingPointsOnSurface(const glm::vec3 hitPoint)
{
    SamplePoints.clear();
    //SamplePoints.append(hitPoint);

    float a = glm::length(transform.T() * glm::vec4(-0.5f,-0.5f,0,1.0f) - transform.T() * glm::vec4(0.5f,-0.5f,0,1.0f));
    float b = glm::length(transform.T() * glm::vec4(-0.5f,-0.5f,0,1.0f) - transform.T() * glm::vec4(-0.5f,0.5f,0,1.0f));

    float ratio  = a/b;

    float delta = 1.0f/60.0f;
    float lim = 10.0f*delta;

    for(float x = glm::max(-0.5f, hitPoint.x - lim * ratio); x<glm::min(0.5f, hitPoint.x+lim*ratio); x+=delta)
        for(float y = glm::max(-0.5f, hitPoint.y - lim); y<glm::min(0.5f, hitPoint.y+lim); y+=delta)
        {
            glm::vec4 temp(transform.T()*glm::vec4(x,y,0.0f,1.0f));
            SamplePoints.push_back(glm::vec3(temp.x/temp.w,temp.y/temp.w,temp.z/temp.w));
        }
}

void SquarePlane::SetNormalMappingValue(Intersection &inter)
{
    if(material!=NULL && material->normal_map != NULL)
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

glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);  // [x - (-0.5)] / 1.0
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
