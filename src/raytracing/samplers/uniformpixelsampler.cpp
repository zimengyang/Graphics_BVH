#include <raytracing/samplers/uniformpixelsampler.h>

UniformPixelSampler::UniformPixelSampler():PixelSampler()
{}

UniformPixelSampler::UniformPixelSampler(int samples):PixelSampler(samples)
{}

QList<glm::vec2> UniformPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;

    float delta = 1.0f / float(samples_sqrt);

    for(float pX = x; pX < x+1; pX += delta)
        for(float pY = y; pY < y+1; pY += delta)
        {
             result.push_back(glm::vec2(pX, pY));
        }

    return result;
}
