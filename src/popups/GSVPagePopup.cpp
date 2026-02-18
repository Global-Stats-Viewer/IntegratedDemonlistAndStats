//geode header
#include <Geode/Geode.hpp>

//other headers
#include "GSVPagePopup.hpp"
#include "../classes/GSVLayer.hpp"
#include "../classes/GSVUtils.hpp"

//geode namespace
using namespace geode::prelude;
using namespace cocos2d;

bool GSVPagePopup::init() {
	if (!Popup::init(220.f, 150.f)) return false;
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	this->setTitle("Go to Page");

	auto layer = typeinfo_cast<CCLayer*>(this->getChildren()->objectAtIndex(0));
	layer->setPosition(284.5f, 220.f);

	//create main layer
	auto mainLayer = CCLayer::create();
	mainLayer->setID("main-layer");

	layer->addChild(mainLayer);
	m_mainLayer = mainLayer;

	//create main menu
	m_pageSelectMenu = CCMenu::create();
	m_pageSelectMenu->setPosition(111.5f, 24.f);
	m_pageSelectMenu->setID("page-select-menu");

	auto confirmBtnSpr = ButtonSprite::create("Go", "goldFont.fnt", "GJ_button_01.png", 0.8f);
	m_confirm = CCMenuItemSpriteExtra::create(confirmBtnSpr, this, menu_selector(GSVPagePopup::confirmPage));
	m_confirm->setID("confirm-btn");
	m_pageSelectMenu->addChild(m_confirm);

	auto resetBtnSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
	m_reset = CCMenuItemSpriteExtra::create(resetBtnSpr, this, menu_selector(GSVPagePopup::resetPage));
	m_reset->setPosition(90.f, 106.f);
	m_reset->setID("reset-btn");
	m_pageSelectMenu->addChild(m_reset);

	auto leftBtnSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
	m_left = CCMenuItemSpriteExtra::create(leftBtnSpr, this, menu_selector(GSVPagePopup::pageLeft));
	m_left->setPosition(-50.f, 56.f);
	m_left->setID("left-btn");
	m_pageSelectMenu->addChild(m_left);

	auto rightBtnSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
	m_right = CCMenuItemSpriteExtra::create(rightBtnSpr, this, menu_selector(GSVPagePopup::pageRight));
	m_right->setPosition(50.f, 56.f);
	m_right->setID("right-btn");
	m_pageSelectMenu->addChild(m_right);

	m_value = TextInput::create(50.f, "1", "bigFont.fnt");
	m_value->setString(std::to_string(m_page).c_str());
	m_value->setCommonFilter(CommonFilter::Int);
	m_value->setMaxCharCount(3);
	m_value->setPositionY(56.f);
	m_value->setID("page-value");
	m_pageSelectMenu->addChild(m_value);

	m_mainLayer->addChild(m_pageSelectMenu);

	return true;
}

void GSVPagePopup::confirmPage(CCObject* sender) {

	GSVLayer* layer = this->getParent()->getChildByType<GSVLayer>(0);
	int page = clampf(numFromString<int>(m_value->getString()).unwrapOr(1), 1, m_maxPages);
    if (page != m_page) layer->loadUsers(page); //if it's the same page don't bother sending another request

	this->removeMeAndCleanup();

	return;
}

void GSVPagePopup::resetPage(CCObject* sender) {
	m_value->setString("1"); //yep, it's that shrimple

	return;
}

void GSVPagePopup::pageLeft(CCObject* sender) {
	int value = numFromString<int>(m_value->getString()).unwrapOr(1);

	value -= 1;
	value = std::max(value, 1);
	m_value->setString(std::to_string(value));
	
	return;
}

void GSVPagePopup::pageRight(CCObject* sender) {
	int value = numFromString<int>(m_value->getString()).unwrapOr(1);

	value += 1;
	value = std::min(value, m_maxPages);
	m_value->setString(std::to_string(value));
	
	return;
}

GSVPagePopup* GSVPagePopup::create(int page, int maxPages) {
	auto ret = new GSVPagePopup();
	ret->m_page = page;
    ret->m_maxPages = maxPages;
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

GSVPagePopup::~GSVPagePopup() {
	this->removeAllChildrenWithCleanup(true);
}
