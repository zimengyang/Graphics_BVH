#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

// return the triangle area of vector a and b
inline float Area(const glm::vec3 &a, const glm::vec3 &b)
{
    return glm::length(glm::cross(a,b)) / 2.0f;
}

float Area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &P)
{
    /*float d0 = glm::distance(position, points[0]);
    float d1 = glm::distance(position, points[1]);
    float d2 = glm::distance(position, points[2]);
    return (normals[0] * d0 + normals[1] * d1 + normals[2] * d2)/(d0+d1+d2);
    */
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], P);
    float A1 = Area(points[0], points[2], P);
    float A2 = Area(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}


// verify x is between [x_min,x_max]
inline bool Between(const float &x, const float &x_min,const float &x_max)
{
    return (x >= x_min) && (x <= x_max);
}
//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r)
{
    Intersection intersection;

    //get the intersection point P of ray and the triangle plane
    if(fequal(glm::dot(plane_normal,r.direction), 0.0f)) // parallel
    {
        return intersection;
    }
    float t = glm::dot(plane_normal, points[0] - r.origin) / glm::dot(plane_normal, r.direction);
    if( t < 0 )    //t < 0: cannot hit anything along positive r.direction
    {
        return intersection;
    }
    glm::vec3 P = r.origin + t*r.direction;

    //calculate areas
    glm::vec3 &P0 = points[0], &P1 = points[1], &P2 = points[2];
    float S =  Area(P1 - P0, P2 - P0) ;
    float S1 = Area(P1 - P, P2 - P) / S;
    float S2 = Area(P1 - P, P0 - P) / S;
    float S3 = Area(P2 - P, P0 - P) / S;

    //determine whether a point P is inside triangle
    if(Between(S1,0.0f,1.0f) && Between(S2,0.0f,1.0f) && Between(S3,0.0f,1.0f) &&
       fequal(S1+S2+S3, 1.0f))
    {
        intersection.t = t;
        intersection.object_hit = this;
        intersection.normal = GetNormal(P);
        intersection.point = P;
        if(material != NULL)
        {
            intersection.color = material->base_color * Material::GetImageColor(GetUVCoordinates(intersection.point), material->texture);
        }

        SetNormalMappingValue(intersection);

    }
    return intersection;
}

void Triangle::SamplingPointsOnSurface(const glm::vec3 hitPoint)
{
    SamplePoints.clear();
    SamplePoints.append(hitPoint);
}

void Triangle::SetNormalMappingValue(Intersection &inter)
{
    if(material!=NULL && material->normal_map != NULL)
    {
        glm::vec3 N = inter.normal;
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
        inter.normal = GetNormal(inter.point);
}
Intersection Mesh::GetIntersection(Ray r)
{
      /// normal intersection for mesh
//    Intersection intersection;
//    float t_min = INFINITY;

//    // for all triangles in mesh
//    for(Triangle* i : faces)
//    {
//        Intersection temp = i->GetIntersection(r);
//        if (temp.t > 0 && temp.t < t_min)
//        {
//            intersection = temp;
//            t_min = temp.t;
//        }
//    }


    /// using bvh accelerating
    Intersection intersection = bvhTree->getIntersection(r);
    if(intersection.t > 0)
    {
        return intersection;
    }
    else
        return Intersection();
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}

//GetUVCoordinates() of Triangle class
glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    float S = Area(points[1] - points[0],points[2] - points[0]);
    float S1 = Area(points[1] - point,points[2]-point) / S;
    float S2 = Area(points[0] - point,points[2]-point) / S;
    float S3 = Area(points[1] - point,points[0]-point) / S;
    return S1 * uvs[0] + S2 * uvs[1] + S3 * uvs[2];
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}

void Triangle::setBBox()
{
    glm::vec3 minB(BBox::mMinVector(points[0],points[1],points[2]));
    glm::vec3 maxB(BBox::mMaxVector(points[0],points[1],points[2]));

    bBox = new BBox(minB, maxB);
    //bBox->geometryAttached = this;
    //bBox->reComputeBoundingBox(transform.T());
    bBox->create();
}

void Mesh::setBBox()
{

    glm::vec3 minB(INFINITY);
    glm::vec3 maxB(-INFINITY);

    for(Triangle* t : faces)
    {
        t->setBBox();
        for(int i=0;i<3;i++)
        {
            minB = BBox::mMinVector(minB, t->points[i]);
            maxB = BBox::mMaxVector(maxB, t->points[i]);
        }
       // t->bBox->reComputeBoundingBox(t->transform.T());
       // t->bBox->create();
    }

    bBox = new BBox(minB, maxB);
    bBox->reComputeBoundingBox(transform.T());
    bBox->create();
    //bBox->geometryAttached = this;
}
