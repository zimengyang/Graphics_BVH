#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <iostream>
#include <functional>

StratifiedPixelSampler::StratifiedPixelSampler():PixelSampler(){}

StratifiedPixelSampler::StratifiedPixelSampler(int samples) : PixelSampler(samples), mersenne_generator(), unif_distribution(0,1){}

QList<glm::vec2> StratifiedPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;
    srand(time(0));

    float delta=1.0f / float(samples_sqrt);
    for(int i=0;i<samples_sqrt;i++)
        for(int j=0;j<samples_sqrt;j++)
        {
            float px = float(i) * delta + x;
            float py = float(j) * delta + y;

            px += (float(rand()) / float(RAND_MAX) * delta);
            py += (float(rand()) / float(RAND_MAX) * delta);
            result.push_back(glm::vec2(px, py));
        }

    return result;
}
