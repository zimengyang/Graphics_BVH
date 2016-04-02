#include <scene/materials/phongmaterial.h>

PhongMaterial::PhongMaterial():
    PhongMaterial(glm::vec3(0.5f, 0.5f, 0.5f))
{}

PhongMaterial::PhongMaterial(const glm::vec3 &color):
    Material(color),
    specular_power(10),
    specular_color(1.0f)
{}

glm::vec3 PhongMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{

    glm::vec3 H = (glm::normalize(outgoing_ray) - glm::normalize(incoming_ray)) / 2.0f;
    float s = glm::pow(glm::dot(glm::normalize(H), isx.normal), specular_power);
    s = (s > 0.0f ? s : 0.0f);
    glm::vec3 S(s,s,s);

    float d = glm::dot(glm::normalize(outgoing_ray), isx.normal);
    d = (d>0?d:0);
    glm::vec3 D(d,d,d);

    return  0.3f * S * specular_color + 0.5f * D * isx.color + 0.2f * isx.color;

}
