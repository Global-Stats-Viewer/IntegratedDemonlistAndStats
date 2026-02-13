#pragma once

#include <Geode/Geode.hpp>

//geode namespace
using namespace geode::prelude;

enum class GSVStatDisplayType {
	Commas,
	Truncated, // probably gonna go unused tbh considering it already exists but i added it anyways
	Scientific // idk if we wanna include this one lol
};

class GSVUtils {
public:
	static std::vector<std::string> substring(std::string string, std::string delim);
	static int safe_stoi(std::string input, int fallback = 0);
	static std::string trimUnicode(std::string input, bool strict = false);
	static std::string formatNumber(double input, GSVStatDisplayType type);
	static void scaleToContentSize(cocos2d::CCNode* node, float width, float height, float minScale = 0.0f);
	static void scaleToContentWidth(cocos2d::CCNode* node, float width, float minScale = 0.0f);
	static void scaleToContentHeight(cocos2d::CCNode* node, float height, float minScale = 0.0f);
};