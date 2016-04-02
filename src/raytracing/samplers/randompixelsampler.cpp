#include "raytracing/samplers/randompixelsampler.h"

RandomPixelSampler::RandomPixelSampler()
{
    samples_sqrt = 1;
}

RandomPixelSampler::RandomPixelSampler(int samples)
{
    samples_sqrt = samples;
}
QList<glm::vec2> RandomPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;
    srand(time(0));

    glm::vec2 sample;
    for(int i=0;i<samples_sqrt*samples_sqrt;i++)
    {
        sample.x = float(x) + float(rand()) / RAND_MAX;
        sample.y = float(y) + float(rand()) / RAND_MAX;

        result.push_back(sample);
    }
    return result;
}
