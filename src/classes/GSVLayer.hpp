#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include <cue/ListNode.hpp>

//geode namespace
using namespace geode::prelude;

class GSVLayer : public CCLayer, LeaderboardManagerDelegate {
protected:
	virtual bool init() override; //add stuff to the layer
	void backButton(CCObject*); //when you press back
	virtual void keyBackClicked() override; //when you press escape

	async::TaskHolder<web::WebResponse> m_listener;

	matjson::Value m_data;
	cue::ListNode* m_list;
	LoadingCircle* m_loadCircle;
	CCMenu* m_pagesMenu;
	CCMenuItemSpriteExtra* m_left;
	CCMenuItemSpriteExtra* m_right;
	CCMenuItemSpriteExtra* m_first;
	CCMenuItemSpriteExtra* m_last;
	CCMenuItemSpriteExtra* m_select;
	CCMenu* m_tabs;
	CCMenu* m_displayMenu;
	TextInput* m_searchInput;
	CCLabelBMFont* m_errorText;

	bool m_usersLoaded = true;
	int m_page = 1;
	//bool m_loadingCancelled = false;

	virtual ~GSVLayer();
public:
	static GSVLayer* create(); //to create the layer

	void callback(CCObject*);

	void reload(CCObject*);

	void pageRight(CCObject*);
	void pageLeft(CCObject*);
	void pageFirst(CCObject*);
	void pageLast(CCObject*);
	void pageGoTo(CCObject*);

	void onTab(CCObject*);
	void onSearch(CCObject*);
	void onFilter(CCObject*);
	void onClearSearch(CCObject*);
	void onToggleDisplay(CCObject*);

	void loadUsers(int);
	void onLoadUsersFinished(matjson::Value const& data);

	void loadLeaderboardFinished(CCArray*, const char*) override;
	void loadLeaderboardFailed(const char*) override;
};

enum class GSVDataType {
	Default,
	Global
};

enum class GSVGamemodeType {
	Classic,
	Platformer,
	Creators
};

enum class GSVFilterType {
	None = 0,

	Stats_Stars = 12,
	Stats_Diamonds = 13,
	Stats_SecretCoins = 15,
	Stats_UserCoins = 14,

	NonDemons_Easy = 2,
	NonDemons_Normal = 3,
	NonDemons_Hard = 4,
	NonDemons_Harder = 5,
	NonDemons_Insane = 6,

	Demons_Count = 1,
	Demons_Easy = 7,
	Demons_Medium = 8,
	Demons_Hard = 9,
	Demons_Insane = 10,
	Demons_Extreme = 11,

	Events_Dailies = 16,
	Events_Weeklies = 17,
	Events_Gauntlets = 18
};

struct GSVFilterFormat {
	std::string search = "";
	GSVDataType dataType = GSVDataType::Global;
	GSVGamemodeType gamemodeType = GSVGamemodeType::Classic;
	GSVFilterType filterType = GSVFilterType::None;
	std::string country = "";
	std::string countrySubdivision = "";
	bool useSecondaryCountry = false;
};

template<>
struct matjson::Serialize<GSVFilterFormat> {
	static Result<GSVFilterFormat> fromJson(matjson::Value const& value) {
		auto gsvff = GSVFilterFormat{
			.search = value["search"].asString().unwrapOr(""),
			.dataType = static_cast<GSVDataType>(value["data-type"].as<int>().unwrapOr(1)),
			.gamemodeType = static_cast<GSVGamemodeType>(value["gamemode-type"].as<int>().unwrapOr(0)),
			.filterType = static_cast<GSVFilterType>(value["filter-type"].as<int>().unwrapOr(0)),
			.country = value["country"].asString().unwrapOr(""),
			.countrySubdivision = value["country-subdivision"].asString().unwrapOr(""),
			.useSecondaryCountry = value["use-secondary-country"].asBool().unwrapOr(false)
		};

		return Ok(gsvff);
	}

	static matjson::Value toJson(GSVFilterFormat const& value) {
		auto obj = matjson::makeObject({
			{ "search", value.search },
			{ "data-type", static_cast<int>(value.dataType) },
			{ "gamemode-type", static_cast<int>(value.gamemodeType) },
			{ "filter-type", static_cast<int>(value.filterType) },
			{ "country", value.country },
			{ "country-subdivision", value.countrySubdivision },
			{ "use-secondary-country", value.useSecondaryCountry }
		});
		return obj;
	}
};