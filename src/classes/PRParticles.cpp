#include <Geode/Geode.hpp>
#include <matjson.hpp>

#include "PRParticles.hpp"

using namespace geode::prelude;

/*
Original Code by Kiba, changed by Mocha

Making the dictionary into a matjson::Value is a waaaay cleaner and less
headache inducing method of doing this imo.
Plus, you don't have to input strings for every value.
*/

CCParticleSystem* PRParticles::trophyGeneral(ccColor3B color, int numParticles) {

    float col_r = color.r / 255.f;
    float col_g = color.g / 255.f;
    float col_b = color.b / 255.f;

    auto dict = matjson::makeObject({
        {"emitterType", 0}, 
        {"duration", -1},
        {"maxParticles", numParticles},
        {"particleLifespan", 2},
        {"particleLifespanVariance", 0.5f},
        {"angle", 90},
        {"angleVariance", 0}, 
        {"sourcePositionx", 0}, 
        {"sourcePositiony", 0}, 
        {"sourcePositionVariancex", 14},
        {"sourcePositionVariancey", 2},
        {"startParticleSize", 4},
        {"finishParticleSize", 1},
        {"startParticleSizeVariance", 1},
        {"speed", 8},
        {"speedVariance", 4},
        {"gravityx", 0}, 
        {"gravityy", 0}, 
        {"startColorRed", col_r},
        {"startColorGreen", col_g},
        {"startColorBlue", col_b},
        {"startColorAlpha", 1.f},
        {"startColorVarianceRed", 0},
        {"startColorVarianceBlue", 0},
        {"startColorVarianceGreen", 0},
        {"startColorVarianceAlpha", 0},
        {"finishColorRed", col_r},
        {"finishColorGreen", col_g},
        {"finishColorBlue", col_b},
        {"finishColorAlpha", 0.f},
        {"finishColorVarianceRed", 0},
        {"finishColorVarianceBlue", 0},
        {"finishColorVarianceGreen", 0},
        {"finishColorVarianceAlpha", 0},
        {"textureFileName", "square.png"} // I CAN'T ESCAPE IT - Mocha
    });

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(jsonToDict(dict), false);

    return emitter;
}

CCParticleSystem* PRParticles::topTrophy0(int numParticles) {
    auto dict = matjson::makeObject({
        {"emitterType", 1},
        {"duration", -1},
        {"maxParticles", numParticles},
        {"particleLifespan", 1.5f},
        {"particleLifespanVariance", 0.5},
        {"angle", 90},
        {"angleVariance", 180},
        {"sourcePositionx", 0},
        {"sourcePositiony", 0}, 
        {"sourcePositionVariancex", 0},
        {"sourcePositionVariancey", 0},
        {"maxRadius", 5},
        {"minRadius", 40},
        {"startParticleSize", 4},
        {"finishParticleSize", 2},
        {"startParticleSizeVariance", 1},
        {"speed", 15},
        {"speedVariance", 8},
        {"gravityx", 0}, 
        {"gravityy", 0}, 
        {"startColorRed", 1.f},
        {"startColorGreen", 1.f},
        {"startColorBlue", 1.f},
        {"startColorAlpha", 0.5f},
        {"startColorVarianceRed", 0.6f},
        {"startColorVarianceBlue", 0.6f},
        {"startColorVarianceGreen", 0.6f},
        {"startColorVarianceAlpha", 0},
        {"finishColorRed", 1.f},
        {"finishColorGreen", 1.f},
        {"finishColorBlue", 1.f},
        {"finishColorAlpha", 0.f},
        {"finishColorVarianceRed", 0.3f},
        {"finishColorVarianceBlue", 0.3f},
        {"finishColorVarianceGreen", 0.3f},
        {"finishColorVarianceAlpha", 0},
        {"textureFileName", "square.png"} // I CAN'T ESCAPE IT - Mocha
    });

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(jsonToDict(dict), false);

    return emitter;
}

CCParticleSystem* PRParticles::topTrophySparkles(int numParticles) {
    auto dict = matjson::makeObject({
        {"emitterType", 0},
        {"duration", -1},
        {"maxParticles", numParticles},
        {"particleLifespan", 1.5f},
        {"particleLifespanVariance", 0.5f},
        {"angle", 0},
        {"angleVariance", 0},
        {"sourcePositionx", 0},
        {"sourcePositiony", 0}, 
        {"sourcePositionVariancex", 10},
        {"sourcePositionVariancey", 10},
        {"startParticleSize", 30},
        {"finishParticleSize", 5},
        {"startParticleSizeVariance", 10},
        {"speed", 0},
        {"speedVariance", 0},
        {"gravityx", 0}, 
        {"gravityy", 0}, 
        {"startColorRed", 1.f},
        {"startColorGreen", 1.f},
        {"startColorBlue", 1.f},
        {"startColorAlpha", 0.6f},
        {"startColorVarianceRed", 0},
        {"startColorVarianceBlue", 0},
        {"startColorVarianceGreen", 0},
        {"startColorVarianceAlpha", 0},
        {"finishColorRed", 1.f},
        {"finishColorGreen", 1.f},
        {"finishColorBlue", 1.f},
        {"finishColorAlpha", 0.f},
        {"finishColorVarianceRed", 0},
        {"finishColorVarianceBlue", 0},
        {"finishColorVarianceGreen", 0},
        {"finishColorVarianceAlpha", 0},
        {"rotationEndVariance", 270},
        {"textureFileName", "sun.png"}
    });

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(jsonToDict(dict), false);

    return emitter;
}

CCDictionary* PRParticles::jsonToDict(matjson::Value j) {
    auto res = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
    
    for (auto [key, value] : j) {
        res->setObject(CCString::create(value.asString().unwrapOrDefault()), key);
    }

    return res;
}