#include <Geode/Bindings.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/loader/Loader.hpp>
#include <matjson.hpp>

#include "../classes/PRParticles.hpp"
#include "Geode/utils/web.hpp"

using namespace geode::prelude;

class $modify(PRProfilePage, ProfilePage) {

    static void onModify(auto &self) {
        self.setHookPriority("ProfilePage::loadPageFromUserInfo", -1);
    }

    struct Fields {
        bool m_hasBeenOpened = false;
    
        ccColor3B m_cColor = {0, 0, 0};
        ccColor3B m_gColor = {0, 0, 0};
        ccColor3B m_dColor = {0, 0, 0};
        ccColor3B m_mColor = {0, 0, 0};

        CCSprite* m_cTrophy = nullptr;
        CCSprite* m_gTrophy = nullptr;
        CCSprite* m_dTrophy = nullptr;
        CCSprite* m_mTrophy = nullptr;

        CCParticleSystem* m_cParticleBase = nullptr;
        CCParticleSystem* m_gParticleBase = nullptr;
        CCParticleSystem* m_dParticleBase = nullptr;
        CCParticleSystem* m_mParticleBase = nullptr;

        CCParticleSystem* m_cParticleExtra = nullptr;
        CCParticleSystem* m_gParticleExtra = nullptr;
        CCParticleSystem* m_dParticleExtra = nullptr;
        CCParticleSystem* m_mParticleExtra = nullptr;

        CCParticleSystem* m_cParticleExtra2 = nullptr;
        CCParticleSystem* m_gParticleExtra2 = nullptr;
        CCParticleSystem* m_dParticleExtra2 = nullptr;
        CCParticleSystem* m_mParticleExtra2 = nullptr;

        async::TaskHolder<web::WebResponse> m_listener;
        async::TaskHolder<web::WebResponse> m_listener2;
        async::TaskHolder<web::WebResponse> m_listener3;

        int m_creatorPosition = 0;
        int m_moonsPosition = 0;
        int m_demonsPosition = 0;

        // CONSTS
        const std::vector<matjson::Value> RANKS = {
            matjson::makeObject({
                {"position", 10000},
                {"color", ccColor3B{ 156, 156, 156 }},
                {"particle-count", -1},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 5000},
                {"color", ccColor3B{ 157, 102, 243 }},
                {"particle-count", -1},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 2500},
                {"color", ccColor3B{ 19, 193, 175 }},
                {"particle-count", -1},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 1000},
                {"color", ccColor3B{ 255, 106, 0 }},
                {"particle-count", -1},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 500},
                {"color", ccColor3B{ 185, 0, 179 }},
                {"particle-count", 7},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 200},
                {"color", ccColor3B{ 26, 160, 255 }},
                {"particle-count", 8},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 100},
                {"color", ccColor3B{ 23, 196, 0 }},
                {"particle-count", 9},
                {"extra-particle-count", -1},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 50},
                {"color", ccColor3B{ 255, 157, 0 }},
                {"particle-count", 10},
                {"extra-particle-count", 15},
                {"sparkle-count", -1}
            }),
            matjson::makeObject({
                {"position", 10},
                {"color", ccColor3B{ 255, 255, 255 }},
                {"particle-count", 11},
                {"extra-particle-count", 20},
                {"sparkle-count", 2}
            }),
            matjson::makeObject({
                {"position", 1},
                {"color", ccColor3B{ 255, 221, 0 }},
                {"particle-count", 12},
                {"extra-particle-count", 25},
                {"sparkle-count", 3}
            }),
            matjson::makeObject({
                {"position", -1},
                {"color", ccColor3B{ 0, 255, 255 }},
                {"particle-count", 13},
                {"extra-particle-count", 30},
                {"sparkle-count", 4}
            })
        };
    };

    void rankingSetValues(int pos) { // Merged everything into one function because they're all the same anyways - Mocha
        PRProfilePage* page = this;

        auto RANKS = m_fields->RANKS;

        for (int i = 0; i < RANKS.size(); i++) {
            auto posCheck = RANKS[i]["position"].as<int>().unwrapOr(-1);
            auto color = RANKS[i]["color"].as<ccColor3B>().unwrapOr(ccColor3B{ 156, 156, 156 });
            auto particleCount = RANKS[i]["particle-count"].as<int>().unwrapOr(-1);
            auto extraParticleCount = RANKS[i]["extra-particle-count"].as<int>().unwrapOr(-1);
            auto sparkleCount = RANKS[i]["sparkle-count"].as<int>().unwrapOr(-1);

            if (pos > posCheck) {
                auto spriteName = fmt::format("trophy_{}.png", i);

                page->m_fields->m_cColor = color;
                page->m_fields->m_cTrophy = CCSprite::createWithSpriteFrameName(Mod::get()->expandSpriteName(spriteName).c_str());
                page->m_fields->m_cParticleBase = (particleCount > -1) ? PRParticles::trophyGeneral(color, particleCount) : nullptr;
                page->m_fields->m_cParticleExtra = (extraParticleCount > -1) ? PRParticles::topTrophy0(extraParticleCount) : nullptr;
                page->m_fields->m_cParticleExtra2 = (sparkleCount > -1) ? PRParticles::topTrophySparkles(sparkleCount) : nullptr;

                break;
            }
        }
    }

    void updateUserRanks(GJUserScore* score) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto layerSize = this->m_mainLayer->getContentSize();
        ccBlendFunc blending = {GL_ONE, GL_ONE};
        int userID = this->m_score->m_userID;
        auto layer = this->m_mainLayer;
        CCSize labelSize = {190, 55};

        auto layerBG = this->m_mainLayer->getChildByID("background");
        auto tempPos = layerBG->getPosition();
        float leftLabelX = convertToWorldSpace(tempPos).x - (layerBG->getContentSize().width / 2.f) + 77.f;
        float rightLabelX = winSize.width - leftLabelX;

        std::vector<int> rankings = {m_fields->m_creatorPosition, score->m_globalRank, m_fields->m_demonsPosition, m_fields->m_moonsPosition};
        std::vector<std::string> rankIDs = {"creator", "global", "demons", "moons"};
        std::vector<std::string> rankTitles = {"Creator", "Global", "Demons", "Moons"};
        std::vector<std::string> rankSprites = {"GJ_hammerIcon_001.png", "GJ_starsIcon_001.png", "GJ_demonIcon_001.png", "GJ_moonsIcon_001.png"};

        for (int i = 0; i < rankings.size(); i++) {
            if (rankings[i] < 1 || layer->getChildByID(fmt::format("{}-rank-tab"_spr, rankIDs[i]))) {
                if (i / 2 == 1) if (auto prev = layer->getChildByID(fmt::format("{}-rank-tab"_spr, rankIDs[i - 2]))) prev->setPositionY(layerSize.height * 0.9f);
                
                continue; // no need to add this ranking if there's no ranking found or ranking already exists
            }

            auto bg = CCScale9Sprite::create("square02_001.png");

            std::string rankString = fmt::format("# {}", rankings[i] + 1);
            rankingSetValues(rankings[i]);
            
            auto rankColor = this->m_fields->m_cColor;
            
            bg->setContentSize(labelSize);
            bg->setZOrder(9);
            bg->setPosition({
                (i % 2 == 0) ? rightLabelX : leftLabelX, 
                (i / 2 == 1) ? (layerSize.height * 0.9f) : (layerSize.height * 0.845f)});
            bg->setOpacity(60);
            bg->setID(fmt::format("{}-rank-tab"_spr, rankIDs[i]));
            auto bgScale = bg->getContentSize();

            auto border = CCScale9Sprite::createWithSpriteFrameName("border.png"_spr);
            border->setContentSize(labelSize);
            border->setZOrder(10);
            border->setPosition(bgScale / 2.f);
            border->setOpacity(200.f);
            border->setColor(rankColor);
            border->setScaleX(-1.f);
            border->setID("rank-border");
            bg->addChild(border);
            
            auto title = CCLabelBMFont::create(fmt::format("{} Rank:", rankTitles[i]).c_str(), "gjFont05.fnt");
            bg->addChild(title);
            title->setScale(0.7f);
            title->setZOrder(12);
            title->setPosition({labelSize.width * (1-0.655f), labelSize.height * 0.69f});
            title->setBlendFunc(blending);
            title->setOpacity(255.f);
            title->setID("rank-title");

            auto rankLabel = CCLabelBMFont::create(rankString.c_str(), "gjFont05.fnt");
            bg->addChild(rankLabel);
            rankLabel->setScale(0.7f);
            rankLabel->setZOrder(12);
            rankLabel->setPosition({labelSize.width * (1-0.655f), labelSize.height * 0.26f});
            rankLabel->setBlendFunc(blending);
            rankLabel->setOpacity(255);

            auto trophySprite = this->m_fields->m_cTrophy;
            bg->addChild(trophySprite);
            trophySprite->setZOrder(12);
            trophySprite->setPosition({labelSize.width * (1-0.19f), labelSize.height * 0.5f});
            trophySprite->setScale(1.5f);
            trophySprite->setOpacity(255);

            if (auto particles = this->m_fields->m_cParticleBase) {
                bg->addChild(particles);
                particles->setPosition({labelSize.width * (1-0.19f), labelSize.height * 0.2f});
                particles->setZOrder(20);
            }
            
            if (auto extraParticles = this->m_fields->m_cParticleExtra) {
                bg->addChild(extraParticles);
                extraParticles->setPosition({labelSize.width * (1-0.19f), labelSize.height * 0.5f});
                extraParticles->setZOrder(11);
            }
            
            if (auto sparkles = this->m_fields->m_cParticleExtra2) {
                bg->addChild(sparkles);
                sparkles->setPosition({labelSize.width * (1-0.19f), labelSize.height * 0.6f});
                sparkles->setZOrder(20);
            }
            

            auto icon = CCSprite::createWithSpriteFrameName(rankSprites[i].c_str());
            bg->addChild(icon);
            icon->setZOrder(13);
            icon->setScale(0.55f);
            icon->setPosition({labelSize.width * (1-0.19f), labelSize.height * 0.65f});

            bg->setScale(0.3f);
            layer->addChild(bg);

            bg->setPositionY(bg->getPositionY() + 1.f);
        }
    }

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        auto layer = this->m_mainLayer;

        if (auto n = layer->getChildByID("global-rank-icon")) n->setVisible(false);
        if (auto n = layer->getChildByID("global-rank-hint")) n->setVisible(false);
        if (auto n = layer->getChildByID("global-rank-label")) n->setVisible(false);
        if (auto n = layer->getChildByID("my-stuff-hint")) n->setVisible(false);
        
        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(layer->getChildByIDRecursive("cvolton.betterinfo/leaderboard-button"))) {
            btn->removeFromParent();
            if (auto leftMenu = layer->getChildByID("left-menu")) {
                leftMenu->addChild(btn);
                leftMenu->updateLayout();
            }
        }

        if (m_fields->m_hasBeenOpened) {
            return;
        }

        m_fields->m_hasBeenOpened = true;

        // Shrink Floor Line
        auto floorLine = static_cast<CCSprite*>(layer->getChildByID("floor-line"));
        floorLine->setScaleX(0.5f);

        //initial profile update with global rank
        updateUserRanks(score);

        /*
        need to find a better api thing before i can finish this, since:
        a) classic and platformer demons are seperate, so i would have to use 2 requests to add everything
        b) + requests for moons & creator points
        
        i tried, but there's nothing else i can really do for now - Mocha
        */

        /*auto req = web::WebRequest();
        req.param("type", "stars");
        req.param("version", "1.0");
        req.param("username", score->m_userName);
        m_fields->m_listener.spawn(
            req.get("https://clarifygdps.com/gdutils/moreleaderboards.php"),
            [&](web::WebResponse value) {
                if (value.ok() && value.json().isOk()) {
                    auto data = value.json().unwrapOrDefault();

                    log::info("data: {}", value.string());

                    updateUserRanks(score);
                }
                else {
                    log::error("Server response failed with Code: {}", value.code());
                }
            }
        );*/
    }
};