#ifndef BESTCANDIDATESAMPLER
#define BESTCANDIDATESAMPLER

#include <raytracing/samplers/pixelsampler.h>

class BestCandidateSampler : public PixelSampler
{
public:
    BestCandidateSampler();
    BestCandidateSampler(int samples);

    virtual QList<glm::vec2> GetSamples(int x, int y);
    virtual QString GetSamplerType() {return QString("bestcandidate");}
};

#endif // BESTCANDIDATESAMPLER

