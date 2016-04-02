#include <scene/materials/lambertmaterial.h>

LambertMaterial::LambertMaterial():Material()
{}

LambertMaterial::LambertMaterial(const glm::vec3 &color):Material(color)
{}

glm::vec3 LambertMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{

    float ar = glm::clamp(glm::dot(isx.normal, glm::normalize(outgoing_ray)), 0.0f, 1.0f);
    return ar * isx.color;
}
