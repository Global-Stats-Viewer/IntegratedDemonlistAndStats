#include <Geode/Geode.hpp>

//geode namespace
using namespace geode::prelude;

class GSVPagePopup : public Popup {
protected:
	bool init() override;
	CCLayer* m_mainLayer;
	virtual ~GSVPagePopup();
	CCMenu* m_pageSelectMenu;
	CCMenuItemSpriteExtra* m_left;
	CCMenuItemSpriteExtra* m_right;
	CCMenuItemSpriteExtra* m_reset;
	CCMenuItemSpriteExtra* m_confirm;
	TextInput* m_value;

	int m_page = 1;
    int m_maxPages = 999;
public:
	static GSVPagePopup* create(int, int);

	void confirmPage(CCObject*);
	void pageLeft(CCObject*);
	void pageRight(CCObject*);
	void resetPage(CCObject*);
};