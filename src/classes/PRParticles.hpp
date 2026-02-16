#pragma once
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class PRParticles {
    public:
        static CCParticleSystem* trophyGeneral(ccColor3B color, int numParticles);
        static CCParticleSystem* topTrophy0(int numParticles);
        static CCParticleSystem* topTrophySparkles(int numParticles);
};