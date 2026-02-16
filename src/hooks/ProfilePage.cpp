#include <Geode/Bindings.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/loader/Loader.hpp>

#include "../classes/PRParticles.hpp"

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

        // CONSTS
        const std::vector<int> POS_CHECK = {10000, 5000, 2500, 1000, 500, 200, 100, 50, 10, 1, -1};
        const std::map<int, ccColor3B> COLOR = {
            {10000, ccColor3B{ 156, 156, 156 }},
            {5000, ccColor3B{ 157, 102, 243 }},
            {2500, ccColor3B{ 19, 193, 175 }},
            {1000, ccColor3B{ 255, 106, 0 }},
            {500, ccColor3B{ 185, 0, 179 }},
            {200, ccColor3B{ 26, 160, 255 }},
            {100, ccColor3B{ 23, 196, 0 }},
            {50, ccColor3B{ 255, 157, 0 }},
            {10, ccColor3B{ 255, 255, 255 }},
            {1, ccColor3B{ 255, 221, 0 }},
            {-1, ccColor3B{ 0, 255, 255 }},
        };
        const std::map<int, int> PARTICLE_COUNT = {
            {10000, -1},
            {5000, -1},
            {2500, -1},
            {1000, -1},
            {500, 7},
            {200, 8},
            {100, 9},
            {50, 10},
            {10, 11},
            {1, 12},
            {-1, 13},
        };
        const std::map<int, int> EXTRA_PARTICLE_COUNT = {
            {10000, -1},
            {5000, -1},
            {2500, -1},
            {1000, -1},
            {500, -1},
            {200, -1},
            {100, -1},
            {50, 15},
            {10, 20},
            {1, 25},
            {-1, 30},
        };
        const std::map<int, int> SPARKLE_COUNT = {
            {10000, -1},
            {5000, -1},
            {2500, -1},
            {1000, -1},
            {500, -1},
            {200, -1},
            {100, -1},
            {50, -1},
            {10, 2},
            {1, 3},
            {-1, 4}
        };
    };

    void rankingSetValues(int pos) { // Merged everything into one function because they're all the same anyways - Mocha
        PRProfilePage* page = this;

        // get consts
        auto POS_CHECK = m_fields->POS_CHECK;
        auto COLOR = m_fields->COLOR;
        auto PARTICLE_COUNT = m_fields->PARTICLE_COUNT;
        auto EXTRA_PARTICLE_COUNT = m_fields->EXTRA_PARTICLE_COUNT;
        auto SPARKLE_COUNT = m_fields->SPARKLE_COUNT;

        // i *could* just add m_fields-> before the params but this looks cleaner

        for (int i = 0; i < POS_CHECK.size(); i++) {
            if (pos > POS_CHECK[i]) {
                auto spriteName = fmt::format("trophy_{}.png", i);

                page->m_fields->m_cColor = COLOR[POS_CHECK[i]];
                page->m_fields->m_cTrophy = CCSprite::createWithSpriteFrameName(Mod::get()->expandSpriteName(spriteName).c_str());
                page->m_fields->m_cParticleBase = (PARTICLE_COUNT[POS_CHECK[i]] > -1) ? PRParticles::trophyGeneral(page->m_fields->m_cColor, PARTICLE_COUNT[POS_CHECK[i]]) : nullptr;
                page->m_fields->m_cParticleExtra = (EXTRA_PARTICLE_COUNT[POS_CHECK[i]] > -1) ? PRParticles::topTrophy0(EXTRA_PARTICLE_COUNT[POS_CHECK[i]]) : nullptr;
                page->m_fields->m_cParticleExtra2 = (SPARKLE_COUNT[POS_CHECK[i]] > -1) ? PRParticles::topTrophySparkles(SPARKLE_COUNT[POS_CHECK[i]]) : nullptr;

                break;
            }
        }
    }


    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        bool creatorExists = false;
        bool globalExists = false;
        bool moonsExists = false;
        bool demonsExists = false;

        if (auto n = this->m_mainLayer->getChildByID("global-rank-icon")) n->setVisible(false);
        if (auto n = this->m_mainLayer->getChildByID("global-rank-hint")) n->setVisible(false);
        if (auto n = this->m_mainLayer->getChildByID("global-rank-label")) n->setVisible(false);
        if (auto n = this->m_mainLayer->getChildByID("my-stuff-hint")) n->setVisible(false);
        
        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(this->m_mainLayer->getChildByIDRecursive("cvolton.betterinfo/leaderboard-button"))) {
            btn->removeFromParent();
            if (auto leftMenu = this->m_mainLayer->getChildByID("left-menu")) {
                leftMenu->addChild(btn);
                leftMenu->updateLayout();
            }
        }

        if (m_fields->m_hasBeenOpened) {
            return;
        }

        m_fields->m_hasBeenOpened = true;

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
        
        // these are set to 0 until we figure out profile stuff - Mocha
        int creatorPosition = 0; //Manager::getPosition(userID);
        int demonsPosition = 0; //Manager::getPositionDemons(userID);
        int moonsPosition = 0; //Manager::getPositionMoons(userID);

        // Shrink Floor Line
        auto floorLine = static_cast<CCSprite*>(layer->getChildByID("floor-line"));
        floorLine->setScaleX(0.5f);

        std::vector<int> rankings = {creatorPosition, score->m_globalRank, demonsPosition, moonsPosition};
        std::vector<std::string> rankIDs = {"creator", "global", "demons", "moons"};
        std::vector<std::string> rankTitles = {"Creator", "Global", "Demons", "Moons"};
        std::vector<std::string> rankSprites = {"GJ_hammerIcon_001.png", "GJ_starsIcon_001.png", "GJ_demonIcon_001.png", "GJ_moonsIcon_001.png"};

        for (int i = 0; i < rankings.size(); i++) {
            if (rankings[i] < 1) {
                if (i / 2 == 1) if (auto prev = layer->getChildByID(fmt::format("{}-rank-tab"_spr, rankIDs[i - 2]))) prev->setPositionY(layerSize.height * 0.9f);
                
                continue; // no need to add this ranking if there's no ranking found
            }

            auto bg = CCScale9Sprite::create("square02_001.png");

            std::string rankString = fmt::format("# {}", rankings[i]);
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
};