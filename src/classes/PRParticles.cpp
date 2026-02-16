#include <Geode/Geode.hpp>
#include <matjson.hpp>

#include "PRParticles.hpp"

using namespace geode::prelude;

/*
Original Code by Kiba, changed by Mocha

Moved particles to .plist files instead of all of it being built here
*/

CCParticleSystem* PRParticles::trophyGeneral(ccColor3B color, int numParticles) {

    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("rankingParticles.plist"_spr);
    float col_r = color.r / 255.f;
    float col_g = color.g / 255.f;
    float col_b = color.b / 255.f;

    dict->setObject(CCString::createWithFormat("%i", numParticles), "maxParticles");
    dict->setObject(CCString::createWithFormat("%f", col_r), "startColorRed");
    dict->setObject(CCString::createWithFormat("%f", col_g), "startColorGreen");
    dict->setObject(CCString::createWithFormat("%f", col_b), "startColorBlue");
    dict->setObject(CCString::createWithFormat("%f", col_r), "finishColorRed");
    dict->setObject(CCString::createWithFormat("%f", col_g), "finishColorGreen");
    dict->setObject(CCString::createWithFormat("%f", col_b), "finishColorBlue");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);

    return emitter;
}

CCParticleSystem* PRParticles::topTrophy0(int numParticles) {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("rankingExtraParticles.plist"_spr);
    dict->setObject(CCString::createWithFormat("%i", numParticles), "maxParticles");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);

    return emitter;
}

CCParticleSystem* PRParticles::topTrophySparkles(int numParticles) {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("rankingSparkles.plist"_spr);
    dict->setObject(CCString::createWithFormat("%i", numParticles), "maxParticles");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);

    return emitter;
}
