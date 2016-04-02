#include "raytracing/samplers/bestcandidatesampler.h"

BestCandidateSampler::BestCandidateSampler()
{
    samples_sqrt = 1;
}

BestCandidateSampler::BestCandidateSampler(int samples)
{
    samples_sqrt = samples;
}

QList<glm::vec2> BestCandidateSampler::GetSamples(int x, int y)
{
    // 64*64 pattern size
    int pattern_size = 64;
    int candidate_number = 5;

    int row = ceil(double(y) / double(pattern_size));
    int col = ceil(double(x) / double(pattern_size));
    QList<glm::vec2> pattern;
    pattern.clear();

    int counts_in_pattern = samples_sqrt * samples_sqrt * pattern_size * pattern_size;
    srand(time(0));
    for(int n=0;n<counts_in_pattern;n++)
    {
        QList<glm::vec2> candidates;
        candidates.clear();
        for(int i = 0;i<candidate_number;i++)
        {
            float a = double(rand()) / RAND_MAX * pattern_size;
            float b = double(rand()) / RAND_MAX * pattern_size;
            candidates.push_back(glm::vec2(a, b));
        }

        //search the closest point
        glm::vec2 best = candidates[0];
        double dis = -INFINITY;
        for(int candi = 1;candi<candidates.size();candi++)
        {
            double closet = INFINITY;
            for(int i=0;i<pattern.size();i++)
                if(glm::length(pattern[i] - candidates[candi]) < closet)
                    closet = glm::length(pattern[i] - candidates[candi]);

            if(closet > dis)
            { // update best candidate
                best = candidates[candi];
                dis = closet;
            }
        }

        pattern.push_back(best);
    }

    // duplicate the pattern
    QList<glm::vec2> result;
    result.clear();
    for(int i=0;i<col;i++)
        for(int j=0;j<row;j++)
        {
            for(glm::vec2 point: pattern)
            {
                float px = float(j*pattern_size) + point.x;
                float py = float(i*pattern_size) + point.y;

                if(px < x && py < y)
                    result.push_back(glm::vec2(px, py));
            }
        }

    return result;
}
