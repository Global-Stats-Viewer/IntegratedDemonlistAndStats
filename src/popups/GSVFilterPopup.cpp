//geode header
#include <Geode/Geode.hpp>

//other headers
#include <Geode/utils/web.hpp>
#include <Geode/utils/JsonValidation.hpp>
#include <Geode/loader/Event.hpp>

#include "../classes/GSVUtils.hpp"
#include "../classes/GSVLayer.hpp"
#include "GSVFilterPopup.hpp"
#include "GSVRegionPopup.hpp"

//geode namespace
using namespace geode::prelude;
using namespace cocos2d;

bool GSVFilterPopup::init() {
    if (!Popup::init(430.f, 280.f)) return false;
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	this->setTitle("Select Filters");

	auto layer = typeinfo_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
	m_mainLayer = layer;

    auto filter = Mod::get()->getSavedValue("gsv-filter", GSVFilterFormat{}); // setup info
    m_filter = (int)filter.filterType;
    m_dataType = (int)filter.dataType;
    m_country = filter.country;
    m_subdivision = filter.countrySubdivision;
    m_useSecondCountry = filter.useSecondaryCountry;

	//create apply menu
	auto mainMenu = CCMenu::create();
	mainMenu->setPosition(215.0f, 24.f);
	mainMenu->setID("main-menu");

	auto applyBtnSpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", 0.8f);
	m_apply = CCMenuItemSpriteExtra::create(applyBtnSpr, this, menu_selector(GSVFilterPopup::onApply));
	m_apply->setID("apply-btn");
	mainMenu->addChild(m_apply);

	auto resetBtnSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
	m_reset = CCMenuItemSpriteExtra::create(resetBtnSpr, this, menu_selector(GSVFilterPopup::resetFilters));
	m_reset->setPosition(195.f, 235.f);
	m_reset->setID("reset-btn");
    m_reset->setOpacity(m_filter <= 0 ? 128 : 255);
    m_reset->setEnabled(m_filter > 0);
    m_reset->setVisible(filter.gamemodeType != GSVGamemodeType::Creators); // hide reset button for creator tab
	mainMenu->addChild(m_reset);

    m_mainLayer->addChild(mainMenu);

	//create filters menu
	m_filtersMenu = CCMenu::create();
	m_filtersMenu->setPosition(m_mainLayer->getContentSize().width / 2.f, (m_mainLayer->getContentSize().height / 2.f) - 25.f);
    m_filtersMenu->setContentHeight(275.f);
    m_filtersMenu->setScale(0.75f);
	m_filtersMenu->setID("filters-menu");
    
    auto filtersLayout = AxisLayout::create(Axis::Column)
        ->setAutoScale(false)
        ->setGap(90.f)
        ->setAxisReverse(true);
    
    m_filtersMenu->setLayout(filtersLayout, true);

    //create checkbox to toggle GSVDataType
    auto globalToggleOnSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto globalToggleOffSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto globalToggleBtn = CCMenuItemToggler::create(globalToggleOffSpr, globalToggleOnSpr, this, menu_selector(GSVFilterPopup::onToggleGlobal));
    globalToggleBtn->setPositionX(-190.f);
    globalToggleBtn->toggle(m_dataType == 0);
    globalToggleBtn->setID("global-toggle-btn");
    auto globalToggleLabel = CCLabelBMFont::create("Show Registered Only", "bigFont.fnt", 0, kCCTextAlignmentLeft);
    globalToggleLabel->setPosition(globalToggleBtn->getPositionX() + 20.f, globalToggleBtn->getPositionY());
    globalToggleLabel->setAnchorPoint({ 0.f, 0.5f });
    globalToggleLabel->setScale(0.325f);
    globalToggleLabel->setID("global-toggle-label");
    mainMenu->addChild(globalToggleLabel);
    mainMenu->addChild(globalToggleBtn);

    //create country select button
    auto countrySelectSpr = CCSprite::create("earth_btn.png"_spr);
    auto countrySelectBtn = CCMenuItemSpriteExtra::create(countrySelectSpr, this, menu_selector(GSVFilterPopup::onCountrySelect));
    countrySelectBtn->setPosition({ 250.f, 225.f });
    countrySelectBtn->setID("country-select-btn");
    mainMenu->addChild(countrySelectBtn);

    //create subdivision select button
    auto subdivisionSelectSpr = CCSprite::create("earth_btn.png"_spr); //TEMPORARY
    auto subdivisionSelectBtn = CCMenuItemSpriteExtra::create(subdivisionSelectSpr, this, menu_selector(GSVFilterPopup::onSubdivisionSelect));
    subdivisionSelectBtn->setPosition({ 250.f, 165.f });
    subdivisionSelectBtn->setID("subdivision-select-btn");
    mainMenu->addChild(subdivisionSelectBtn);

    // create sections
    const std::vector<std::string> sectionIDs = {"stats", "difficulty", "events"};
    // first value of filter = sprite name, second value = filter type
    const matjson::Value sectionInfo = matjson::parse(R"(
        {
            "stats": {
                "name": "Stats",
                "filters": [
                    {
                        "sprite": "GJ_bigStar_noShadow_001.png",
                        "id": 12
                    },
	                {
                        "sprite": "GJ_diamondsIcon_001.png", 
                        "id": 13
                    },
                    {
                        "sprite": "secretCoinUI_001.png", 
                        "id": 15
                    },
	                {
                        "sprite": "secretCoinUI2_001.png", 
                        "id": 14
                    }
                ]
            },
            "difficulty": {
                "name": "Difficulty",
                "filters": [
                    {
                        "sprite": "difficulty_01_btn_001.png", 
                        "id": 2
                    },
                    {
                        "sprite": "difficulty_02_btn_001.png", 
                        "id": 3
                    },
                    {
                        "sprite": "difficulty_03_btn_001.png", 
                        "id": 4
                    },
                    {
                        "sprite": "difficulty_04_btn_001.png", 
                        "id": 5
                    },
                    {
                        "sprite": "difficulty_05_btn_001.png", 
                        "id": 6
                    },
                    {
                        "sprite": "difficulty_06_btn_001.png", 
                        "id": 1
                    },
                    {
                        "sprite": "difficulty_07_btn2_001.png", 
                        "id": 7
                    },
                    {
                        "sprite": "difficulty_08_btn2_001.png", 
                        "id": 8
                    },
                    {
                        "sprite": "difficulty_06_btn2_001.png", 
                        "id": 9
                    },
                    {
                        "sprite": "difficulty_09_btn2_001.png", 
                        "id": 10
                    },
                    {
                        "sprite": "difficulty_10_btn2_001.png", 
                        "id": 11
                    }
                ]
            },
            "events": {
                "name": "Events",
                "filters": [
                    {
                        "sprite": "gj_dailyCrown_001.png", 
                        "id": 16
                    },
                    {
                        "sprite": "gj_weeklyCrown_001.png", 
                        "id": 17
                    },
                    {
                        "sprite": "island_demon_001.png", 
                        "id": 18
                    }
                ]
            }
        }
    )").unwrapOrDefault(); // NOTE: THESE SPRITES ARE NOT FINAL

    // change star sprite to moons if platformer mode
    /*if (filter.gamemodeType == GSVGamemodeType::Platformer) {
        sectionInfo["stats"]["filters"][0][0].asString().unwrapOrDefault() = "GJ_moonsIcon_001.png";
    }*/

    for (int i = 0; i < sectionIDs.size(); i++) {
        if (filter.gamemodeType == GSVGamemodeType::Creators) { // don't even bother adding filters for the creators section
            auto creatorLabel = CCLabelBMFont::create("Filters are not available for the Creators Tab", "bigFont.fnt");
            creatorLabel->setScale(0.5f);
            creatorLabel->setID("creators-filter-label");
            creatorLabel->setPosition(m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f);
            m_mainLayer->addChild(creatorLabel);
            break;
        }
        if (filter.gamemodeType == GSVGamemodeType::Platformer && sectionIDs[i] == "events") continue; // skip events for platformer since they're classic only
        
        auto filterSection = CCMenu::create();
        filterSection->setID(fmt::format("{}-section", sectionIDs[i]));
        filterSection->setContentHeight(0.f);

        auto sectionBackground = CCScale9Sprite::create("square02_001.png");
        sectionBackground->setOpacity(128);
        sectionBackground->setZOrder(-1);
        filterSection->addChild(sectionBackground);

        auto filtersContainer = CCMenu::create();
        filtersContainer->setContentHeight(0.f);
        filtersContainer->setPositionY(40.f);
        filtersContainer->setID(fmt::format("{}-filters-container", sectionIDs[i]));

        auto filtersLayout = AxisLayout::create();
        filtersLayout->setAxis(Axis::Row)
            ->setGap(10.f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisAlignment(AxisAlignment::Start)
            ->setGrowCrossAxis(true)
            ->setAutoGrowAxis(150.f)
            ->ignoreInvisibleChildren(true);
        
        filtersContainer->setLayout(filtersLayout);

        //finally add the buttons
        for (auto f : sectionInfo[sectionIDs[i]]["filters"].as<std::vector<matjson::Value>>().unwrapOr(std::vector<matjson::Value>())) {
            auto filterToggleOnSpr = CCSprite::createWithSpriteFrameName(f["sprite"].asString().unwrapOr("GJ_moonsIcon_001.png").c_str());
            if (filter.gamemodeType == GSVGamemodeType::Platformer && f["sprite"].asString().unwrapOr("GJ_moonsIcon_001.png") == "GJ_bigStar_noShadow_001.png") {
                //stupid solution but it works so idc
                filterToggleOnSpr = CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
            }
            GSVUtils::scaleToContentSize(filterToggleOnSpr, 40.f, 40.f, 0.9f);
            
            auto filterToggleOffSpr = CCSprite::createWithSpriteFrameName(f["sprite"].asString().unwrapOr("GJ_moonsIcon_001.png").c_str());
            if (filter.gamemodeType == GSVGamemodeType::Platformer && f["sprite"].asString().unwrapOr("GJ_moonsIcon_001.png") == "GJ_bigStar_noShadow_001.png") {
                //stupid solution but it works so idc
                filterToggleOffSpr = CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
            }
            filterToggleOffSpr->setOpacity(128);
            GSVUtils::scaleToContentSize(filterToggleOffSpr, 40.f, 40.f, 0.9f);

            auto filterToggleBtn = CCMenuItemToggler::create(filterToggleOffSpr, filterToggleOnSpr, this, menu_selector(GSVFilterPopup::onSelectFilter));
            filterToggleBtn->setTag(f["id"].as<int>().unwrapOr(0));
            filterToggleBtn->toggle(filterToggleBtn->getTag() == m_filter);

            if (Mod::get()->getSettingValue<bool>("compact-demon-filter")) {
                if ((m_filter >= 7 && m_filter <= 11) || m_filter == 1) { // demon filters
                    if ((filterToggleBtn->getTag() >= 2 && filterToggleBtn->getTag() <= 6)) {
                        filterToggleBtn->setVisible(false);
                    }
                } else {
                    if ((filterToggleBtn->getTag() >= 7 && filterToggleBtn->getTag() <= 11)) {
                        filterToggleBtn->setVisible(false);
                    }
                }
            }

            filtersContainer->addChild(filterToggleBtn);

            m_filterItems.push_back(filterToggleBtn);
        }

        filtersContainer->updateLayout(false);
        filterSection->addChild(filtersContainer);
        if (!Loader::get()->isModLoaded("alphalaneous.happy_textures")) {
            // happy textures fixes problems with vanilla scale9 sprites, so we have to do some trickery to make it look fine in vanilla
            sectionBackground->setContentSize({(filtersContainer->getContentWidth() + 10.f) * 2.f, (filtersContainer->getContentHeight() + 10.f) * 2.f});
            sectionBackground->setScale(0.5f);
        }
        else sectionBackground->setContentSize({filtersContainer->getContentWidth() + 10.f, filtersContainer->getContentHeight() + 10.f});
        sectionBackground->setPosition(filtersContainer->getPosition());

        auto sectionLabel = CCLabelBMFont::create(sectionInfo[sectionIDs[i]]["name"].asString().unwrapOrDefault().c_str(), "bigFont.fnt");
        sectionLabel->setPosition(filtersContainer->getPositionX(), filtersContainer->getPositionY() + (filtersContainer->getContentHeight() / 2.f) + 10.f);
        sectionLabel->setAnchorPoint({0.5f, 0.f});
        sectionLabel->setScale(0.5f);
        sectionLabel->setID(fmt::format("{}-section-label", sectionIDs[i]));
        filterSection->addChild(sectionLabel);

        m_filtersMenu->addChild(filterSection);
    }

    m_filtersMenu->updateLayout();
    layer->addChild(m_filtersMenu);

	return true;
}

void GSVFilterPopup::onApply(CCObject* sender) {

    auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});
    filter.filterType = static_cast<GSVFilterType>(m_filter);
    filter.dataType = static_cast<GSVDataType>(m_dataType);
    filter.country = m_country;
    filter.countrySubdivision = m_subdivision;
    filter.useSecondaryCountry = m_useSecondCountry;

    Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", filter);

	GSVLayer* layer = this->getParent()->getChildByType<GSVLayer>(0);
    layer->loadUsers(1);

	this->removeMeAndCleanup();

	return;
}

void GSVFilterPopup::resetFilters(CCObject* sender) {
    
    m_filter = static_cast<int>(GSVFilterType::None);
    onSelectFilter(sender);

    m_reset->setOpacity(128);
    m_reset->setEnabled(false);

	return;
}

void GSVFilterPopup::onSelectFilter(CCObject* sender) {
    auto item = static_cast<CCMenuItemToggler*>(sender);
    
    m_filter = (item->isToggled()) ? 0 : item->getTag();

    // all filters should be cleared except for the selected one
    for (auto child : m_filterItems) {
        auto btn = static_cast<CCMenuItemToggler*>(child);
        if (btn->isToggled() && (btn->getTag() != item->getTag())) btn->toggle(false);

        // compacting stuff
        if (Mod::get()->getSettingValue<bool>("compact-demon-filter")) {
            if ((m_filter >= 7 && m_filter <= 11) || m_filter == 1) { // demon filters
                if ((btn->getTag() >= 2 && btn->getTag() <= 6)) {
                    btn->setVisible(false);
                } else {
                    btn->setVisible(true);
                }
            } else {
                if ((btn->getTag() >= 7 && btn->getTag() <= 11)) {
                    btn->setVisible(false);
                }
                else {
                    btn->setVisible(true);
                }
            }

            // now we get the parents and resize the background + update layout
            auto parent = this->getChildByIDRecursive("difficulty-section");
            auto bg = parent->getChildByType<CCScale9Sprite>(0);
            auto container = parent->getChildByID("difficulty-filters-container");
            auto label = parent->getChildByID("difficulty-section-label");
            if (!container || !bg) continue;
            container->updateLayout(false);
            if (!Loader::get()->isModLoaded("alphalaneous.happy_textures")) { 
                // happy textures fixes problems with vanilla scale9 sprites, so we have to do some trickery to make it look fine in vanilla
                bg->setContentSize({(container->getContentWidth() + 10.f) * 2.f, (container->getContentHeight() + 10.f) * 2.f});
                bg->setScale(0.5f);
            }
            else bg->setContentSize({container->getContentWidth() + 10.f, container->getContentHeight() + 10.f});
            label->setPositionY(container->getPositionY() + (container->getContentHeight() / 2.f) + 10.f);
            m_filtersMenu->updateLayout();
        }
    }

    m_reset->setOpacity(m_filter == 0 ? 128 : 255);
    m_reset->setEnabled(m_filter != 0);

    return;
}

void GSVFilterPopup::onToggleGlobal(CCObject* sender) {
    auto item = static_cast<CCMenuItemToggler*>(sender);
    
    m_dataType = (m_dataType == static_cast<int>(GSVDataType::Global)) ? static_cast<int>(GSVDataType::Default) : static_cast<int>(GSVDataType::Global);

    return;
}

void GSVFilterPopup::onCountrySelect(CCObject* sender) {
    GSVRegionPopup::create(false)->show();
    return;
}

void GSVFilterPopup::onSubdivisionSelect(CCObject* sender) {
    GSVRegionPopup::create(true)->show();
    return;
}

void GSVFilterPopup::updateCountry() {
    //called by country popup to update button sprites
    //might remove idk
    return;
}

GSVFilterPopup* GSVFilterPopup::create() {
	auto ret = new GSVFilterPopup();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

GSVFilterPopup::~GSVFilterPopup() {
	this->removeAllChildrenWithCleanup(true);
}
