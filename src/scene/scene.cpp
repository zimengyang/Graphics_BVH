#include <scene/scene.h>

#include <scene/geometry/cube.h>
#include <scene/geometry/sphere.h>
#include <scene/geometry/mesh.h>
#include <scene/geometry/square.h>

#include <raytracing/samplers/uniformpixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <raytracing/samplers/randompixelsampler.h>

#include <scene/materials/lambertmaterial.h>
#include <scene/materials/phongmaterial.h>

Scene::Scene()
{
    pixel_sampler = new UniformPixelSampler();
}

void Scene::SetCamera(const Camera &c)
{
    camera = Camera(c);
    camera.create();
    film.SetDimensions(c.width, c.height);
}

void Scene::CreateTestScene()
{
    Material* lambert1 = new LambertMaterial(glm::vec3(1, 0, 0));
    Material* lambert2 = new LambertMaterial(glm::vec3(0, 1, 0));

    Cube* c = new Cube();
    c->material = lambert1;
    c->transform = Transform(glm::vec3(1,0,0), glm::vec3(40,20,45), glm::vec3(1,1,1));
    c->create();
    c->setBBox();
    this->objects.append(c);

    Sphere* s = new Sphere();
    s->material = lambert2;
    s->transform = Transform(glm::vec3(-1,0,0), glm::vec3(30,50,30), glm::vec3(1,2,1));
    s->create();
    s->setBBox();
    this->objects.append(s);

    unsigned int w = 400, h = 400;
    camera = Camera(w, h);
    camera.near_clip = 0.1f;
    camera.far_clip = 100.0f;
    camera.create();
    film = Film(w, h);
}

void Scene::Clear()
{
    for(Geometry *g : objects)
    {
        QString name = g->getGeometryName();
        if(QString::compare(name,QString("Mesh")) == 0)
            BVHNode::releaseTree(((Mesh*)g)->bvhTree);

        delete g;
    }
    objects.clear();
    lights.clear();
    for(Material *m : materials)
    {
        delete m;
    }
    materials.clear();
    camera = Camera();
    film = Film();
    delete pixel_sampler;

    // settting the sampler properties
    pixel_sampler = new UniformPixelSampler(1);
    //pixel_sampler = new StratifiedPixelSampler(4);
    //pixel_sampler = new RandomPixelSampler(4);

}
