//geode header
#include <Geode/Geode.hpp>

//other headers
#include <Geode/utils/web.hpp>
#include <Geode/utils/JsonValidation.hpp>
#include <Geode/loader/Event.hpp>

#include "../classes/GSVUtils.hpp"
#include "GSVFilterPopup.hpp"
#include "GSVRegionPopup.hpp"

//geode namespace
using namespace geode::prelude;
using namespace cocos2d;

bool GSVRegionPopup::init() {
    if (!Popup::init(320.f, 280.f)) return false;
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    this->setTitle(m_useSubdivisions ? "Select Subdivision" : "Select Country");

    auto layer = typeinfo_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
    m_mainLayer = layer;

    //create search menu
    auto searchMenu = CCMenu::create();
    searchMenu->setPosition(m_mainLayer->getContentWidth() / 2.f, 225.f);
    searchMenu->setID("search-menu");

    m_searchInput = TextInput::create(200.f, "", "bigFont.fnt");
    m_searchInput->setPlaceholder("Search...");
    m_searchInput->setCallback([this](const std::string& text) {this->onUpdate(text);});
    m_searchInput->setID("search-input");
    searchMenu->addChild(m_searchInput);

    auto clearBtnSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    auto clearBtn = CCMenuItemSpriteExtra::create(clearBtnSpr, this, menu_selector(GSVRegionPopup::onClear));
    clearBtn->setID("clear-btn");

    m_mainLayer->addChild(searchMenu);

    m_loadCircle = LoadingCircle::create();
	m_loadCircle->m_parentLayer = this;
	m_loadCircle->show();

    auto req = web::WebRequest();
    std::string dataURL = (m_useSubdivisions ? "https://api.globalstatsviewer.com/api/getsubdivisions" : "https://api.globalstatsviewer.com/api/getcountries");
	m_listener.spawn(
        req.get(dataURL),
        [&](web::WebResponse value) {
			if (value.ok() && value.json().isOk() && !value.json().isErr()) {
				m_data = value.json().unwrapOrDefault();

				m_loadCircle->fadeAndRemove();
				m_isLoaded = true;
				log::info("Country Data loaded!");

				onUpdate("");
			}
			else {
				m_loadCircle->fadeAndRemove();

				FLAlertLayer::create(
					"ERROR",
					fmt::format("Something went wrong...\nCode: {}", value.code()).c_str(),
					"OK"
				)->show();

				this->removeMeAndCleanup();
			}
        }
    );

    return true;
}

void GSVRegionPopup::onSelect(CCObject* sender) {
    //handle region selection
}

void GSVRegionPopup::onClear(CCObject* sender) {
    //handle clearing selection
}

void GSVRegionPopup::onUpdate(const std::string& text) {
    if (m_list) m_list->removeMeAndCleanup();

    std::string searchLower = text;
    std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

    //setup cells
    CCArray* cells = CCArray::create();
    for (matjson::Value i : m_data.as<std::vector<matjson::Value>>().unwrapOr(std::vector<matjson::Value>())) {
        std::string code = i["code"].asString().unwrapOr("");
        std::string displayName = i["display_name"].asString().unwrapOr("???");
        int id = i["id"].asInt().unwrapOr(0);
        std::string name = i["name"].asString().unwrapOr("");
        std::string host = ""; // for subdivisions

        if (m_useSubdivisions) {
            code = i["abbrev"].asString().unwrapOr("");

            auto splitCode = GSVUtils::substring(code, "-");
            log::info("{}", splitCode);
            if (splitCode.size() != 2) continue;
            host = splitCode[0];
        }

        std::string codeLower = code;
        std::transform(codeLower.begin(), codeLower.end(), codeLower.begin(), ::tolower);
        std::string displayNameLower = displayName;
        std::transform(displayNameLower.begin(), displayNameLower.end(), displayNameLower.begin(), ::tolower);
        std::string nameLower = name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        std::string hostLower = host;
        std::transform(hostLower.begin(), hostLower.end(), hostLower.begin(), ::tolower);

        auto cell = CCMenu::create();
        cell->setContentSize({ 200.f, 30.f });
        cell->setTag(id);

        auto label = CCLabelBMFont::create(displayName.c_str(), "bigFont.fnt");
        label->setPosition(30.f, cell->getContentHeight() / 2.f);
        label->setScale(0.5f);
        if (label->getContentWidth() > 400.f) GSVUtils::scaleToContentWidth(label, 200.f, 0.1f);
        label->setAnchorPoint({ 0.f, 0.5f });
        label->setID("region-label");
        cell->addChild(label);

        auto flagSpr = LazySprite::create({20.f, 20.f}, false);
        flagSpr->initWithSpriteFrameName(Mod::get()->expandSpriteName(fmt::format("{}.png", codeLower)).data());
        flagSpr->setAutoResize(false);
        flagSpr->setLoadCallback([flagSpr](Result<> res) {
            if (res) {
                log::info("Sprite loaded successfully!");
            } else {
                log::error("Sprite failed to load, setting fallback: {}", res.unwrapErr());
                flagSpr->initWithSpriteFrameName("unknown-flag.png"_spr);
            }
        });
        flagSpr->setPosition({5.f, cell->getContentHeight() / 2.f});
        flagSpr->setScale(0.75f);
        flagSpr->setAnchorPoint({ 0.f, 0.5f });
        flagSpr->setID("region-flag");
        cell->addChild(flagSpr);

        auto selectBtnSpr = ButtonSprite::create("Select", 0.75f);
        selectBtnSpr->setScale(0.7f);
        auto selectBtn = CCMenuItemSpriteExtra::create(selectBtnSpr, this, menu_selector(GSVRegionPopup::onSelect));
        selectBtn->setPosition(265.f, cell->getContentHeight() / 2.f);
        selectBtn->setID("select-btn");
        cell->addChild(selectBtn);

        /*
        now we filter and add the cells based on search text
        Search Text is:
        - NOT case sensitive
        - Matched with code, id, display name, and name (all transformed to lower case)
        If the search text at least matches one of those, it's added
        If there's no text in the search bar, just continue

        For Subdivisions:
        - Only show subdivisions that match the country if the country is selected, otherwise show all
        */

        if (searchLower.size() > 0) {
            if (codeLower.find(searchLower) == std::string::npos &&
                displayNameLower.find(searchLower) == std::string::npos &&
                nameLower.find(searchLower) == std::string::npos &&
                std::to_string(id).find(searchLower) == std::string::npos) {
                continue;
            }
        }

        if (m_useSubdivisions && m_countryCode != "" && host != m_countryCode) continue;

        cells->addObject(cell);
    }
    
    //create list view
    m_list = ListView::create(cells, 30.f, 300.f, 180.f);
    m_list->setPosition(10.f, 15.f);
    m_list->setID("region-list");

    m_mainLayer->addChild(m_list);
}

GSVRegionPopup* GSVRegionPopup::create(bool useSubdivisions, std::string countryCode) {
    auto ret = new GSVRegionPopup();
    ret->m_useSubdivisions = useSubdivisions;
    ret->m_countryCode = countryCode;
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

GSVRegionPopup::~GSVRegionPopup() {
	this->removeAllChildrenWithCleanup(true);
}