#include <scene/materials/material.h>
#include <QColor>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    base_color(color),
    emissive(false),
    reflectivity(0),
    refract_idx_in(0),
    refract_idx_out(0)
{
    texture = NULL;
    normal_map = NULL;
}

glm::vec3 Material::GetImageColor(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        int X = image->width() * uv_coord.x;
        X = (X >= image->width()? X-1: X);
        X = (X < 0? 0: X);
        int Y = image->height() * (1.0f - uv_coord.y);
        Y = (Y >= image->height()? Y-1: Y);
        Y = (Y < 0? 0: Y);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue())/255.0f;
    }
}
