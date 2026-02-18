//geode header
#include <Geode/Geode.hpp>

#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>

#include "GSVUtils.hpp"

//geode namespace
using namespace geode::prelude;

std::vector<std::string> GSVUtils::substring(std::string s, std::string d) {
	std::vector<std::string> res;
	std::string delim = d;
	std::string token = "";
	for (int i = 0; i < s.size(); i++) {
		bool flag = true;
		for (int j = 0; j < delim.size(); j++) {
			if (s[i + j] != delim[j]) flag = false;
		}
		if (flag) {
			if (token.size() > 0) {
				res.push_back(token);
				token = "";
				i += delim.size() - 1;
			}
		}
		else {
			token += s[i];
		}
	}
	res.push_back(token);
	return res;
};

std::string GSVUtils::trimUnicode(std::string input, bool strict) {
	std::string result = "";
	std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-_=+[]{}\\|'\";:.>,</?`~ ";
	if (strict) allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 ";
	bool lastWasSpace = false;

	for (auto c : input) {
		for (auto ch : allowedChars) {
			if (c == ch) {
				if (c == ' ') {
					if (!lastWasSpace) result.push_back(c);
					lastWasSpace = (c == ' ');
				} else {
					result.push_back(c);
					lastWasSpace = false;
				}
				break;
			}
		}
	}

	// get those trailing spaces outta there
	for (auto c : result) {
		if (result.back() == ' ') { result.pop_back(); }
		else if (result.front() == ' ') { result.erase(result.begin()); }
		else { break; }
	}

	return result;
};

std::string GSVUtils::formatNumber(double input, GSVStatDisplayType type) {
	if (input < 1000.0f) return std::to_string(static_cast<int>(floor(input)));
	
	switch (type) {
	case GSVStatDisplayType::Commas: {
		std::string numStr = std::to_string(static_cast<long long>(input));
		int insertPosition = numStr.length() - 3;
		while (insertPosition > 0) {
			numStr.insert(insertPosition, ",");
			insertPosition -= 3;
		}
		return numStr;
	}
	case GSVStatDisplayType::Truncated: {
		const char* suffixes[] = { "", "k", "M", "B", "T" };
		int suffixIndex = 0;
		while (input >= 1000.0 && suffixIndex < 4) {
			input /= 1000.0;
			suffixIndex++;
		}
		char buffer[50];
		std::snprintf(buffer, sizeof(buffer), "%.1f%s", input, suffixes[suffixIndex]);
		return std::string(buffer);
	}
	case GSVStatDisplayType::Scientific: {
		char buffer[50];
		std::snprintf(buffer, sizeof(buffer), "%.2e", input);
		return std::string(buffer);
	}
	default:
		return std::to_string(input);
	}
};

void GSVUtils::scaleToContentSize(cocos2d::CCNode* node, float width, float height, float minScale) {
	auto contentSize = node->getContentSize();
	float scaleX = width / contentSize.width;
	float scaleY = height / contentSize.height;
	float scale = std::min(scaleX, scaleY);
	if (minScale > 0.f && scale < minScale) scale = minScale;
	node->setScale(scale);

	return;
}

void GSVUtils::scaleToContentWidth(cocos2d::CCNode* node, float width, float minScale) {
	auto contentSize = node->getContentSize();
	float scaleX = width / contentSize.width;
	if (minScale > 0.f && scaleX < minScale) scaleX = minScale;
	node->setScale(scaleX);
}

void GSVUtils::scaleToContentHeight(cocos2d::CCNode* node, float height, float minScale) {
	auto contentSize = node->getContentSize();
	float scaleY = height / contentSize.height;
	if (minScale > 0.f && scaleY < minScale) scaleY = minScale;
	node->setScale(scaleY);

	return;
}