#pragma once
#include <raytracing/samplers/pixelsampler.h>
#include <random>

class RandomPixelSampler : public PixelSampler
{
public:
    RandomPixelSampler();
    RandomPixelSampler(int samples);

    virtual QList<glm::vec2> GetSamples(int x, int y);
    virtual QString GetSamplerType() {return QString("random");}
};
