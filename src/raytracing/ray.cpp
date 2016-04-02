#include <raytracing/ray.h>
#include <la.h>
Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o),
    direction(glm::normalize(d)),
    transmitted_color(1,1,1)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(glm::vec3(o), glm::vec3(d))
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction)
{
    transmitted_color = r.transmitted_color;
}
Ray::Ray():
    origin(0,0,0),
    direction(0,1,0),
    transmitted_color(1,1,1)
{}
Ray Ray::GetTransformedCopy(const glm::mat4 &T) const
{
    // TODO: Implement this correctly!

    Ray newRay;
    // First: make origin point and direction vector homogeneous.
    glm::vec4 homoOrigin(origin, 1.0f);
    glm::vec4 homoDirection(direction, 0.0f);

    // Second : apply transformation matrix
    glm::vec4 newOrigin(T * homoOrigin);
    glm::vec4 newDirection(T * homoDirection);

    // Third : get new homogeneous origin and direction
    newRay.origin = glm::vec3(
                newOrigin.x / newOrigin.w,
                newOrigin.y / newOrigin.w,
                newOrigin.z / newOrigin.w);

    newRay.direction = glm::vec3(
                newDirection.x,
                newDirection.y,
                newDirection.z);

    // Fourth : normalize direction vector
    newRay.direction = glm::normalize(newRay.direction);

    return newRay;
}
