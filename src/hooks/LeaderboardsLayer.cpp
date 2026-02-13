/*

Change this later, this is just for testing stuff and is not at all polished lol
~ Mocha

*/

//geode header
#include <Geode/Geode.hpp>

//other headers
#include "../classes/GSVLayer.hpp"
#include <Geode/modify/LeaderboardsLayer.hpp>

//geode namespace
using namespace geode::prelude;
using namespace cocos2d;

//add button to creator layer
class $modify(LeaderboardsLayer) {

	static void onModify(auto & self) {
		static_cast<void>(self.setHookPriority("LeaderboardsLayer::init", -42));
	}

	bool init(LeaderboardType p0, LeaderboardStat p1) {
		if (!LeaderboardsLayer::init(p0, p1)) return false;
		
        auto size = CCDirector::sharedDirector()->getWinSize();

		//add a button that opens the GSV menu
		auto menu = CCMenu::create();
		menu->setPosition({ 0, 0 });

        auto spr = CircleButtonSprite::create(CCLabelBMFont::create("GSV", "bigFont.fnt"));
        auto gsvBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(GSVLayer::callback));
        gsvBtn->setID("gsv-button"_spr);
        gsvBtn->setPosition({ size.width - 50, size.height - 50 });
        gsvBtn->setZOrder(10);
		
        menu->addChild(gsvBtn);
		this->addChild(menu);

		return true;
	}
};