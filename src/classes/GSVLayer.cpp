//geode header
#include <Geode/Geode.hpp>

//other headers
#include <Geode/utils/web.hpp>
#include <Geode/utils/JsonValidation.hpp>
#include <Geode/loader/Event.hpp>

#include "GSVLayer.hpp"
#include "../popups/GSVPagePopup.hpp"
#include "../popups/GSVFilterPopup.hpp"
#include "GSVUtils.hpp"

//geode namespace
using namespace geode::prelude;
using namespace cocos2d;

void GSVLayer::callback(CCObject*) {
	auto scene = CCScene::create(); // creates the scene
	auto gsvLayer = GSVLayer::create(); //creates the layer

	scene->addChild(gsvLayer);

	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene)); // push transition

	return;
}

void GSVLayer::keyBackClicked() {
	//m_loadingCancelled = true;

	CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);

	return;
};

void GSVLayer::backButton(CCObject* sender) {
	keyBackClicked();

	return;
};

bool GSVLayer::init() {
	if (!CCLayer::init()) return false;

	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	auto bg = createLayerBG();
	bg->setZOrder(-10);
	bg->setID("bg");
	this->addChild(bg);

	auto lCornerSprite = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
	lCornerSprite->setAnchorPoint({ 0, 0 });
	lCornerSprite->setID("left-corner-sprite");
	this->addChild(lCornerSprite);

	auto rCornerSprite = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
	rCornerSprite->setAnchorPoint({ 1, 0 });
	rCornerSprite->setPosition({ size.width, 0 });
	rCornerSprite->setFlipX(true);
	rCornerSprite->setID("right-corner-sprite");
	this->addChild(rCornerSprite);

	//error text
	m_errorText = CCLabelBMFont::create("Something went wrong...", "bigFont.fnt");
	m_errorText->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
	m_errorText->setPosition({ size.width / 2, size.height / 2 });
	m_errorText->setScale(0.6f);
	m_errorText->setZOrder(100);
	m_errorText->setVisible(false);
	m_errorText->setID("error-text");
	this->addChild(m_errorText);

	//back button
	auto backSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
	auto backButton = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(GSVLayer::backButton));
	auto backMenu = CCMenu::create();
	backMenu->addChild(backButton);
	backMenu->setPosition({ 25, size.height - 25 });
	backMenu->setZOrder(2);
	backMenu->setID("back-menu");
	this->addChild(backMenu);

	//info button
	auto infoMenu = CCMenu::create();
	auto infoButton = InfoAlertButton::create( // reformatted this and also removed the link since you can't add links on the info layer for some reason
		"Stats Viewer", 
		"This is the leaderboard for the Integrated Stats Viewer. You can search for any stat leaderboard here along an aggregate ranking of all users' careers.\n\nIn-game stats from the <cj>Updated Leaderboard</c>. Level completion data from <cr>Pointercrate</c>, <cr>AREDL</c>, and the <cl>Pemonlist</c>.", 
		1.0f
	);
	infoMenu->setPosition({ 25, 25 });
	infoMenu->setZOrder(2);
	infoMenu->addChild(infoButton);
	infoMenu->setID("info-menu");
	this->addChild(infoMenu);

	//reload menu
	auto reloadMenu = CCMenu::create();
	reloadMenu->setPosition({ size.width - 30.f, 30.f });
	auto reloadBtnSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
	auto reloadBtn = CCMenuItemSpriteExtra::create(reloadBtnSprite, this, menu_selector(GSVLayer::reload));
	reloadBtn->setPosition({ 0, 0 });
	reloadMenu->addChild(reloadBtn);
	reloadMenu->setID("reload-menu");
	this->addChild(reloadMenu);

	//pages menu
	m_pagesMenu = CCMenu::create();
	m_pagesMenu->setPosition({ 0, 0 });
	m_pagesMenu->setID("pages-menu");

	m_left = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), this, menu_selector(GSVLayer::pageLeft));
	m_left->setPosition(24.f, size.height / 2);
	m_left->setVisible(false);
	m_left->setID("page-left-btn");
	m_pagesMenu->addChild(m_left);

	auto rightBtnSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	rightBtnSpr->setFlipX(true);
	m_right = CCMenuItemSpriteExtra::create(rightBtnSpr, this, menu_selector(GSVLayer::pageRight));
	m_right->setPosition(size.width - 24.0f, size.height / 2);
	m_right->setVisible(false);
	m_right->setID("page-right-btn");
	m_pagesMenu->addChild(m_right);

	auto firstBtnSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	auto firstBtnSpr2 = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	firstBtnSpr->setScale(0.5f);
	firstBtnSpr2->setAnchorPoint({0.5f, 0.f});
	firstBtnSpr2->setPositionX(-5.f);
	firstBtnSpr->addChild(firstBtnSpr2);
	m_first = CCMenuItemSpriteExtra::create(firstBtnSpr, this, menu_selector(GSVLayer::pageFirst));
	m_first->getChildByType<CCSprite>(0)->setPosition({14.5f, 10.f});
	m_first->setPosition(24.f, (size.height / 2) - 40.f);
	m_first->setVisible(false);
	m_first->setID("page-first-btn");
	m_pagesMenu->addChild(m_first);

	auto lastBtnSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	auto lastBtnSpr2 = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
	lastBtnSpr->setFlipX(true);
	lastBtnSpr2->setFlipX(true);
	lastBtnSpr->setScale(0.5f);
	lastBtnSpr2->setAnchorPoint({0.5f, 0.f});
	lastBtnSpr2->setPositionX(-5.f);
	lastBtnSpr2->setZOrder(-1);
	lastBtnSpr->addChild(lastBtnSpr2);
	m_last = CCMenuItemSpriteExtra::create(lastBtnSpr, this, menu_selector(GSVLayer::pageLast));
	m_last->getChildByType<CCSprite>(0)->setPosition({14.5f, 10.f});
	m_last->setPosition(size.width - 24.f, (size.height / 2) - 40.f);
	m_last->setVisible(false);
	m_last->setID("page-last-btn");
	m_pagesMenu->addChild(m_last);

	this->addChild(m_pagesMenu);

	//list
	// CustomListView::create(CCArray::create(), BoomListType::Default, 220.0f, 358.0f)
	auto items = CCArray::create();
	auto item = CCLabelBMFont::create("hello", "bigFont.fnt");
	items->addObject(item);

	// auto listNode = ListView::create(items);
	CCSize listSize { 358.0f, 220.0f };
	m_list = cue::ListNode::create(listSize, cue::Brown, cue::ListBorderStyle::Levels);
	m_list->setZOrder(2);
	m_list->setPosition(size / 2 - m_list->getContentSize() / 2);
	m_list->setAnchorPoint({ 0, 0 });
	auto ogTopBorder = m_list->getChildByType<cue::ListBorder>(0)->getChildByType<CCSprite>(0);
	ogTopBorder->setVisible(false);
	auto topBorder = CCSprite::createWithSpriteFrameName("GJ_table_top02_001.png");
	topBorder->setPosition(ogTopBorder->getPosition());
	topBorder->setAnchorPoint(ogTopBorder->getAnchorPoint());
	topBorder->setZOrder(ogTopBorder->getZOrder());
	topBorder->setID("top-border-alt");
	m_list->addChild(topBorder);
	this->addChild(m_list);

	//tabs
	m_tabs = CCMenu::create();
	m_tabs->setPosition({ size.width / 2, 298.75f });
	m_tabs->setZOrder(3);
	m_tabs->setID("tabs-menu");
	m_tabs->setVisible(false);

	// @geode-ignore(unknown-resource)
	auto backTabSprite = CCSprite::create("geode.loader/tab-gradient.png"); // use geode loader assets for the tab bg/stencil
	auto tabClippingNode = CCClippingNode::create();
	// @geode-ignore(unknown-resource)
	auto tabStencil = CCSprite::create("geode.loader/tab-gradient-mask.png");
	tabStencil->setAnchorPoint({ 0, 0.5f });
	tabClippingNode->setStencil(tabStencil);
	tabClippingNode->setAnchorPoint({ 0, 0.5f });
	tabClippingNode->setPosition({ 0.f, 18.f });
	tabClippingNode->addChild(backTabSprite);
	tabClippingNode->setZOrder(-1);
	tabClippingNode->setAlphaThreshold(0.0f);

	auto classicBtn = TabButton::create(TabBaseColor::Unselected, TabBaseColor::Selected, "Classic", this, menu_selector(GSVLayer::onTab));
	classicBtn->setPositionX(-110.f);
	classicBtn->setID("classic-tab");
	classicBtn->setTag(static_cast<int>(GSVGamemodeType::Classic));
	classicBtn->toggle(true);
	classicBtn->m_onButton->addChild(tabClippingNode);
	m_tabs->addChild(classicBtn);

	auto platBtn = TabButton::create(TabBaseColor::Unselected, TabBaseColor::Selected, "Platformer", this, menu_selector(GSVLayer::onTab));
	platBtn->setPositionX(0.f);
	platBtn->setID("platformer-tab");
	platBtn->setTag(static_cast<int>(GSVGamemodeType::Platformer));
	platBtn->toggle(false);
	platBtn->m_onButton->addChild(tabClippingNode);
	m_tabs->addChild(platBtn);

	auto creatorsBtn = TabButton::create(TabBaseColor::Unselected, TabBaseColor::Selected, "Creators", this, menu_selector(GSVLayer::onTab));
	creatorsBtn->setPositionX(110.f);
	creatorsBtn->setID("creators-tab");
	creatorsBtn->setTag(static_cast<int>(GSVGamemodeType::Creators));
	creatorsBtn->toggle(false);
	creatorsBtn->m_onButton->addChild(tabClippingNode);
	m_tabs->addChild(creatorsBtn);

	this->addChild(m_tabs);

	// list display settings menu
	m_displayMenu = CCMenu::create();
	m_displayMenu->setZOrder(3);
	m_displayMenu->setAnchorPoint({0.5f, 1.f});
	m_displayMenu->setContentSize({0.f, 180.f});
	m_displayMenu->setPosition(72.5f, 250.f);
	m_displayMenu->setID("display-menu");

	auto displayMenuLayout = AxisLayout::create(Axis::Column)
		->setAxisReverse(true)
		->setAxisAlignment(AxisAlignment::End)
		->setAutoScale(false);
	
	m_displayMenu->setLayout(displayMenuLayout);

	auto statDisplayType = static_cast<GSVStatDisplayType>(Mod::get()->getSavedValue<int>("gsv-stat-display-type", 0));
	auto showStatIcons = Mod::get()->getSavedValue<bool>("gsv-show-stat-icons", true);

	auto statFormatTopNode = CCLabelBMFont::create("123", "bigFont.fnt");
	statFormatTopNode->setScale(0.5f);
	auto statFormatBtnOffSprite = EditorButtonSprite::create(statFormatTopNode, EditorBaseColor::Green, EditorBaseSize::Normal);
	statFormatBtnOffSprite->setScale(0.8f);
	auto statFormatBtnOnSprite = EditorButtonSprite::create(statFormatTopNode, EditorBaseColor::Cyan, EditorBaseSize::Normal);
	statFormatBtnOnSprite->setScale(0.8f);
	auto statFormatBtn = CCMenuItemToggler::create(statFormatBtnOffSprite, statFormatBtnOnSprite, this, menu_selector(GSVLayer::onToggleDisplay));
	statFormatBtn->setID("stat-format-btn");
	statFormatBtn->setTag(0);
	statFormatBtn->toggle(statDisplayType == GSVStatDisplayType::Truncated);
	m_displayMenu->addChild(statFormatBtn);

	auto statIconsBtnOffSprite = EditorButtonSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png", 0.75f, EditorBaseColor::Green, EditorBaseSize::Normal);
	statIconsBtnOffSprite->setScale(0.8f);
	auto statIconsBtnOnSprite = EditorButtonSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png", 0.75f, EditorBaseColor::Cyan, EditorBaseSize::Normal);
	statIconsBtnOnSprite->setScale(0.8f);
	auto statIconsBtn = CCMenuItemToggler::create(statIconsBtnOffSprite, statIconsBtnOnSprite, this, menu_selector(GSVLayer::onToggleDisplay));
	statIconsBtn->setID("stat-icons-btn");
	statIconsBtn->setTag(1);
	statIconsBtn->toggle(showStatIcons);
	m_displayMenu->addChild(statIconsBtn);

	m_displayMenu->updateLayout();
	this->addChild(m_displayMenu);

	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});
	filter.search = ""; // the search bar should be empty on init
	filter.gamemodeType = GSVGamemodeType::Classic; // default to classic on init
	Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", filter);

	loadUsers(1);

	this->setKeyboardEnabled(true);
	this->setKeypadEnabled(true);

	return true;
}

void GSVLayer::reload(CCObject* sender) {
	m_errorText->setVisible(false);

	if (m_usersLoaded) {
		loadUsers(m_page);
	}

	return;
}

void GSVLayer::pageLeft(CCObject* sender) {
	m_page -= 1;

	loadUsers(m_page);

	return;
}

void GSVLayer::pageRight(CCObject* sender) {
	m_page += 1;

	loadUsers(m_page);

	return;
}

void GSVLayer::pageFirst(CCObject* sender) {
	m_page = 1;

	loadUsers(m_page);

	return;
}

void GSVLayer::pageLast(CCObject* sender) {
	m_page = m_data["info"]["additional_info"]["number_of_pages"].as<int>().unwrapOr(0);

	loadUsers(m_page);

	return;
}

void GSVLayer::pageGoTo(CCObject* sender) {
	GSVPagePopup::create(m_page, m_data["info"]["additional_info"]["number_of_pages"].as<int>().unwrapOr(999))->show();

	return;
}

void GSVLayer::onTab(CCObject* sender) {
	auto tabBtn = static_cast<TabButton*>(sender);

	if (tabBtn->isToggled()) return; // already selected, don't need to reload here
	
	for (auto child : m_tabs->getChildrenExt()) {
		auto btn = static_cast<TabButton*>(child);
		btn->toggle(false);
	}

	tabBtn->toggle(true);

	//change gamemode filter based on tab
	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});
	filter.gamemodeType = static_cast<GSVGamemodeType>(tabBtn->getTag());

	Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", filter);

	loadUsers(1);

	return;
}

void GSVLayer::onSearch(CCObject* sender) {
	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});

	if (filter.search == m_searchInput->getString()) return; // if the value is the same it's not gonna give different results smh

	filter.search = m_searchInput->getString();

	Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", filter);

	loadUsers(1);

	return;
}

void GSVLayer::onFilter(CCObject* sender) {
	GSVFilterPopup::create()->show();

	return;
}

void GSVLayer::onClearSearch(CCObject* sender) {
	m_searchInput->setString("");
	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});
	filter.search = "";
	Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", filter);
	loadUsers(1);
	return;
}

void GSVLayer::onToggleDisplay(CCObject* sender) {
	auto btn = static_cast<CCMenuItemToggler*>(sender);
	auto tag = btn->getTag();

	switch (tag)
	{
	case 0: { // stat format toggle
		Mod::get()->setSavedValue<int>("gsv-stat-display-type", btn->isToggled() ? (int)GSVStatDisplayType::Commas : (int)GSVStatDisplayType::Truncated);
		break;
	}
	case 1: {
		Mod::get()->setSavedValue<bool>("gsv-show-stat-icons", !btn->isToggled());
		break;
	}
	default: break;
	}

	if (m_usersLoaded) {
		loadUsers(m_page);
	}

	return;
}

void GSVLayer::loadUsers(int page) {
	m_list->clear();
	if (!m_usersLoaded) return; // if you somehow manage to call this while it's still loading, congrats
	log::info("load");

	m_pagesMenu->setVisible(false);

	m_usersLoaded = false;

	m_loadCircle = LoadingCircle::create();
	m_loadCircle->m_parentLayer = this;
	m_loadCircle->show();

	for (auto child : m_tabs->getChildrenExt()) { //disable switching tabs while loading
		auto btn = static_cast<TabButton*>(child);
		btn->setEnabled(false);
	}

	m_page = page;

	if (auto sm = m_list->getChildByID("search-menu")) sm->setVisible(false);

	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});
	if (filter.gamemodeType == GSVGamemodeType::Platformer && (
		filter.filterType == GSVFilterType::Events_Dailies ||
		filter.filterType == GSVFilterType::Events_Weeklies ||
		filter.filterType == GSVFilterType::Events_Gauntlets)
	) filter.filterType = GSVFilterType::None; // temporarily set the filter to none if the filter is an event type in platformer

	std::map<GSVGamemodeType, std::string> gamemodeMap = {
		{ GSVGamemodeType::Classic, "classic" },
		{ GSVGamemodeType::Platformer, "platformer" },
		{ GSVGamemodeType::Creators, "creator" }
	};

	auto req = web::WebRequest();
	req.param("page", page);
	req.param("dataType", (filter.dataType == GSVDataType::Global) ? "global" : "default");
	req.param("gamemode", gamemodeMap[filter.gamemodeType]);
	req.param("filter", (int)filter.filterType);
	req.param("country", filter.country);
	req.param("subdivision", filter.countrySubdivision);
	req.param("includeSecondary", filter.useSecondaryCountry);
	req.param("search", filter.search);

	std::string dataURL = "https://api.globalstatsviewer.com/api/leaderboard";
	m_listener.spawn(
		req.get(dataURL),
		[&](web::WebResponse value) {
			if (value.ok() && value.json().isOk() && !value.json().isErr()) {
				m_data = value.json().unwrapOrDefault();

				m_loadCircle->fadeAndRemove();
				m_usersLoaded = true;
				log::info("Data loaded!");

				onLoadUsersFinished(m_data);
			}
			else {
				m_loadCircle->fadeAndRemove();

				m_errorText->setCString(fmt::format("Something went wrong...\n(Code: {}, JSON Error: {})", value.code(), value.json().isErr()).c_str());
				m_errorText->setVisible(true);

				m_usersLoaded = true;
				Mod::get()->setSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{}); // reset filter on error since filters can't be changed
			}
		}
	);

	//OLD TEMPORARY TESTING CODE
	
	/*std::vector<int> tempList = {71, 11839560, 34499, 3223}; //account ids to test

	//m_right->setVisible(m_IDs.size() > 10);

	auto scores = CCArray::create();
	auto glm = GameLevelManager::sharedState();
	std::vector<std::string> res;
	glm->m_leaderboardManagerDelegate = this;
	glm->m_leaderboardState = LeaderboardState::Global;

	for (auto id : tempList) res.push_back(std::to_string(id));

	auto searchObject = GJSearchObject::create(SearchType::Users, string::join(res, ","));
	//glm->getLeaderboardScores(searchObject->getKey());

	log::info("{}", res);
	log::info("{}", searchObject->getKey());
	//log::info("{}", users);

	if (users) {
		loadLevelsFinished(users, "");
	}
	else {
		glm->getLeaderboardScores(searchObject->getKey());
	}

	auto gsm = GameStatsManager::sharedState();
	
	for (int i = page * 100; i < std::max((page * 100) + 100, (int)tempList.size()); i++) {
		auto score = glm->userInfoForAccountID(tempList[i]);

		if (!score) { //fallback for if the user info is empty
			auto icon = gsm->m_accountIDForIcon.find({tempList[i], UnlockType::GJItem});
			auto user = gsm->m_usernameForAccountID.find(tempList[i]);
			
			score = GJUserScore::create();
			score->m_accountID = tempList[i];
			score->m_userID = glm->userIDForAccountID(tempList[i]);
            score->m_userName = user != gsm->m_usernameForAccountID.end() ? user->second : "-";
			score->m_iconID = icon != gsm->m_accountIDForIcon.end() ? icon->second : 1;
			score->m_iconType = IconType::Cube;
		}

		score->m_playerRank = i + 1;
		scores->addObject(score);

		//store for later
		glm->storeUserName(score->m_userID, score->m_accountID, score->m_userName);
	}

	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	m_list = GJListLayer::create(CustomListView::create(scores, BoomListType::Score, 220.0f, 358.0f), "Stats Viewer", {194, 114, 62, 255}, 358.0f, 220.0f, 0);
	m_list->setZOrder(2);
	m_list->setPosition(size / 2 - m_list->getContentSize() / 2);
	this->addChild(m_list);

	m_usersLoaded = true;

	m_loadCircle->fadeAndRemove();*/

	return;
}

void GSVLayer::onLoadUsersFinished(matjson::Value const& data) {
	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();
	
	auto filter = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{});

	m_pagesMenu->setVisible(m_usersLoaded);
	m_left->setVisible(m_page > 1);
	m_right->setVisible(data["info"]["additional_info"]["number_of_pages"].as<int>().unwrapOr(0) > m_page + 1);
	m_first->setVisible(m_left->isVisible());
	m_last->setVisible(m_right->isVisible());
	if (m_select) m_select->removeMeAndCleanup();
	if (auto pagesLabel = m_pagesMenu->getChildByID("pages-label")) pagesLabel->removeMeAndCleanup();
	// if (m_list) m_list->removeMeAndCleanup();
	m_tabs->setVisible(true);

	for (auto child : m_tabs->getChildrenExt()) { //reenable tab buttons
		auto btn = static_cast<TabButton*>(child);
		btn->setEnabled(true);
	}
	
	//recreate select button
	auto selectBtnSpr = ButtonSprite::create(std::to_string(m_page).c_str(), "bigFont.fnt", "GJ_button_02.png", 0.8f);
	selectBtnSpr->setScale(0.7f);
	selectBtnSpr->m_BGSprite->setContentSize({ 40.f, 40.f });
	m_select = CCMenuItemSpriteExtra::create(selectBtnSpr, this, menu_selector(GSVLayer::pageGoTo));
	m_select->setPosition(size.width - 24.f, size.height - 30.f);
	m_select->setID("page-select-btn");
	m_pagesMenu->addChild(m_select);

	//pages label
	auto pagesLabel = CCLabelBMFont::create(fmt::format("Page {} of {}", m_page, data["info"]["additional_info"]["number_of_pages"].as<int>().unwrapOr(0)).c_str(), "goldFont.fnt");
	pagesLabel->setID("pages-label");
	pagesLabel->setAnchorPoint({1.f, 0.f});
	pagesLabel->setScale(0.45f);
	pagesLabel->setPosition(size.width - 7.f, size.height - 14.f);
	m_pagesMenu->addChild(pagesLabel);

	//recreate list and add tabs

	m_list->setVisible(true);

	auto scrollLayer = m_list->getChildByType<ScrollLayer>(0);
	if (scrollLayer) {
		scrollLayer->setContentHeight(190.f);
		scrollLayer->scrollToTop();
		scrollLayer->setVisible(true);
	}

	// TODO: proper error handling; this will crash if we receive invalid data lol
	// i think i prevented that, it'll at least return a blank list if we get invalid data - mocha
	for (auto user : data["leaderboard_data"].asArray().unwrapOr(std::vector<matjson::Value>{})) {
		
		// SETUP DATA
		auto profileData = user["profile"];
		auto countryData = user["country_data"];
		auto hardestData = user["hardest_data"];
		auto pointsTypeData = user["points_type"];
		auto lbPosition = user["position"].as<int>().unwrapOr(0);
		auto points = user["points"].as<float>().unwrapOr(0.f);
		auto truncPoints = user["truncated_points"].asString().unwrapOr("");

		// CELL STUFF
		auto cell = CCLayer::create();
		cell->setContentHeight(30.f);
		auto statDisplayType = static_cast<GSVStatDisplayType>(Mod::get()->getSavedValue<int>("gsv-stat-display-type", 0));

		auto placementLabel = CCLabelBMFont::create(std::to_string(lbPosition).c_str(), "goldFont.fnt");
		placementLabel->setAnchorPoint({ 0.f, 0.5f });
		placementLabel->setScale(0.5f);
		placementLabel->setPosition({ 10.f, 15.f });
		placementLabel->setID("placement-label");
		cell->addChild(placementLabel);

		auto usernameLabel = CCLabelBMFont::create(GSVUtils::trimUnicode(profileData["username"].asString().unwrapOr("Unknown"), true).c_str(), "goldFont.fnt"); // will be a button eventually
		usernameLabel->setAnchorPoint({ 0.f, 0.5f });
		usernameLabel->setScale(0.5f);
		usernameLabel->setPosition({ 60.f, 15.f });
		usernameLabel->setID("username-label");
		cell->addChild(usernameLabel);

		std::map<GSVFilterType, std::string> statIconMap = {
			{ GSVFilterType::None, "GJ_pointsIcon_001.png" },
			{ GSVFilterType::Stats_Stars, "GJ_starsIcon_001.png" },
			{ GSVFilterType::Stats_Diamonds, "GJ_diamondsIcon_001.png" },
			{ GSVFilterType::Stats_SecretCoins, "secretCoinUI_001.png" },
			{ GSVFilterType::Stats_UserCoins, "secretCoinUI2_001.png" },
			{ GSVFilterType::NonDemons_Easy, "diffIcon_01_btn_001.png" },
			{ GSVFilterType::NonDemons_Normal, "diffIcon_02_btn_001.png" },
			{ GSVFilterType::NonDemons_Hard, "diffIcon_03_btn_001.png" },
			{ GSVFilterType::NonDemons_Harder, "diffIcon_04_btn_001.png" },
			{ GSVFilterType::NonDemons_Insane, "diffIcon_05_btn_001.png" },
			{ GSVFilterType::Demons_Count, "diffIcon_06_btn_001.png"}, // TODO: change this icon to something better since rn this will just match hard demon count - mocha
			{ GSVFilterType::Demons_Easy, "diffIcon_07_btn_001.png"},
			{ GSVFilterType::Demons_Medium, "diffIcon_08_btn_001.png"},
			{ GSVFilterType::Demons_Hard, "diffIcon_06_btn_001.png"},
			{ GSVFilterType::Demons_Insane, "diffIcon_09_btn_001.png"},
			{ GSVFilterType::Demons_Extreme, "diffIcon_10_btn_001.png"},
			{ GSVFilterType::Events_Dailies, "gj_dailyCrown_001.png" },
			{ GSVFilterType::Events_Weeklies, "gj_weeklyCrown_001.png" },
			{ GSVFilterType::Events_Gauntlets, "island_demon_001.png" }
		};

		if (Mod::get()->getSavedValue<bool>("gsv-show-stat-icons", true)) {
			if (static_cast<int>(filter.filterType) < 0) filter.filterType = GSVFilterType::None; // because for some reason negative filter types exist???
			// no seriously, how does this happen???
			// i ALWAYS set the filters to "0" or None, -1 shouldn't even ever happen because 0 IS THE DEFAULT 
			// afaik there's no other code in the mod that sets the filter to "-1" - mocha

			auto statIcon = CCSprite::createWithSpriteFrameName(statIconMap[filter.filterType].c_str());
			if (filter.filterType == GSVFilterType::Events_Gauntlets) statIcon = CCSprite::create("island_demon_001.png");
			if (filter.filterType == GSVFilterType::Stats_Stars && filter.gamemodeType == GSVGamemodeType::Platformer) statIcon = CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
			if (filter.gamemodeType == GSVGamemodeType::Creators) statIcon = CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
			GSVUtils::scaleToContentSize(statIcon, 18.f, 18.f);
			statIcon->setPosition({ 340.f, 15.f });
			statIcon->setAnchorPoint({ 1.f, 0.5f });
			statIcon->setID("stat-icon");
			cell->addChild(statIcon);

			auto pointsLabel = CCLabelBMFont::create(((statDisplayType == GSVStatDisplayType::Truncated) ? truncPoints : GSVUtils::formatNumber(points, statDisplayType)).c_str(), "bigFont.fnt");
			if (points < 1000.f) pointsLabel->setString(fmt::format("{}", static_cast<int>(floor(points))).c_str());
			pointsLabel->setAnchorPoint({ 1.f, 0.5f });
			pointsLabel->setScale(0.5f);
			pointsLabel->setPosition({ 315.f, 15.f });
			pointsLabel->setID("points-label");
			cell->addChild(pointsLabel);
		}
		else {
			auto pointsStr = fmt::format("{} {}", ((statDisplayType == GSVStatDisplayType::Truncated) ? truncPoints : GSVUtils::formatNumber(points, statDisplayType)), pointsTypeData["display_filter"].asString().unwrapOr("Points"));
			if (points < 1000.f) pointsStr = fmt::format("{} {}", static_cast<int>(floor(points)), pointsTypeData["display_filter"].asString().unwrapOr("Points"));
			auto pointsLabel = CCLabelBMFont::create(pointsStr.c_str(), "bigFont.fnt");
			pointsLabel->setAnchorPoint({ 1.f, 0.5f });
			pointsLabel->setScale(0.5f);
			pointsLabel->setPosition({ 340.f, 15.f });
			pointsLabel->setID("points-label");
			cell->addChild(pointsLabel);
		}

		m_list->addCell(cell);
	}

	//search menu (layout taken with permission from jasmine, modified by me) - mocha
	auto searchQuery = Mod::get()->getSavedValue<GSVFilterFormat>("gsv-filter", GSVFilterFormat{}).search;
	if (m_list->getChildByID("search-menu")) m_list->getChildByID("search-menu")->removeMeAndCleanup();

	auto searchMenu = CCMenu::create();
    searchMenu->setContentSize({ 356.0f, 30.0f });
    searchMenu->setPosition({ 0.0f, 190.0f });
    searchMenu->setID("search-menu");
    m_list->addChild(searchMenu);

    auto searchBackground = CCLayerColor::create({ 194, 114, 62, 255 }, 356.0f, 30.0f);
    searchBackground->setID("search-background");
	searchBackground->setZOrder(-1);
    searchMenu->addChild(searchBackground);

	auto searchBtnSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
	searchBtnSprite->setScale(0.7f);
    auto searchButton = CCMenuItemSpriteExtra::create(searchBtnSprite, this, menu_selector(GSVLayer::onSearch));
    searchButton->setPosition({ 337.0f, 15.0f });
    searchButton->setID("search-button");
    searchMenu->addChild(searchButton);

	auto filterBtnSprite = EditorButtonSprite::createWithSpriteFrameName("GJ_filterIcon_001.png", 0.75f, EditorBaseColor::Green, EditorBaseSize::Normal);
	filterBtnSprite->setScale(0.7f);
	auto filterButton = CCMenuItemSpriteExtra::create(filterBtnSprite, this, menu_selector(GSVLayer::onFilter));
	filterButton->setPosition({ 312.0f, 15.f });
	filterButton->setContentSize(filterBtnSprite->getScaledContentSize());
	filterButton->setID("filter-button");
	searchMenu->addChild(filterButton);
	
	auto clearBtnSprite = EditorButtonSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png", 0.75f, EditorBaseColor::Green, EditorBaseSize::Normal);
	clearBtnSprite->setScale(0.7f);
	clearBtnSprite->setCascadeOpacityEnabled(true);
	clearBtnSprite->setOpacity((searchQuery == "" || searchQuery.empty()) ? 135 : 255);
	auto clearButton = CCMenuItemSpriteExtra::create(clearBtnSprite, this, menu_selector(GSVLayer::onClearSearch));
	clearButton->setPosition({ 287.0f, 15.f });
	clearButton->setContentSize(clearButton->getScaledContentSize());
	clearButton->setID("clear-button");
	clearButton->setEnabled(searchQuery != "" && !searchQuery.empty());
	searchMenu->addChild(clearButton);

    m_searchInput = TextInput::create(353.3f, "Search...");
	m_searchInput->setString(searchQuery.c_str());
	m_searchInput->setAnchorPoint({ 0.f, 0.5f });
    m_searchInput->setPosition({ 7.0f, 15.0f });
    m_searchInput->setTextAlign(TextInputAlign::Left);
    auto inputNode = m_searchInput->getInputNode();
    inputNode->setLabelPlaceholderScale(0.53f);
    inputNode->setMaxLabelScale(0.53f);
    m_searchInput->setScale(0.75f);
    m_searchInput->setID("search-input");
    searchMenu->addChild(m_searchInput);

	m_loadCircle->fadeAndRemove();

	return;
}

void GSVLayer::loadLeaderboardFinished(CCArray* scores, const char* key) { // UNUSED
	log::info("{}", scores);
	log::info("{}", key);
	
	m_usersLoaded = true;

	auto director = CCDirector::sharedDirector();
	auto size = director->getWinSize();

	// m_list = GJListLayer::create(CustomListView::create(scores, BoomListType::Score, 220.0f, 358.0f), "Stats Viewer", {194, 114, 62, 255}, 358.0f, 220.0f, 0);
	// m_list->setZOrder(2);
	// m_list->setPosition(size / 2 - m_list->getContentSize() / 2);
	// this->addChild(m_list);

	m_usersLoaded = true;

	m_loadCircle->fadeAndRemove();

	return;
}

void GSVLayer::loadLeaderboardFailed(const char* key) { // UNUSED
	m_errorText->setVisible(true);

	m_loadCircle->fadeAndRemove();

	return;
}

GSVLayer* GSVLayer::create() {
	auto pRet = new GSVLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet); //don't crash if it fails
	return nullptr;
}

GSVLayer::~GSVLayer() {
    this->removeAllChildrenWithCleanup(true);
}