#include <Geode/Geode.hpp>

//geode namespace
using namespace geode::prelude;

class GSVFilterPopup : public Popup {
protected:
	bool init() override;
    virtual ~GSVFilterPopup();
	CCLayer* m_mainLayer;
	CCMenu* m_filtersMenu;
    CCArrayExt<CCNode*> m_filterItems;
	CCMenuItemSpriteExtra* m_apply;
    CCMenuItemSpriteExtra* m_reset;

    int m_filter = 0;
    int m_dataType = 1;
    std::string m_country = "";
    std::string m_subdivision = "";
    bool m_useSecondCountry = false;

public:
	static GSVFilterPopup* create();

    void onSelectFilter(CCObject*);
	void onApply(CCObject*);
    void resetFilters(CCObject*);
    void onToggleGlobal(CCObject*);
    void onCountrySelect(CCObject*);
    void onSubdivisionSelect(CCObject*);
    void updateCountry(); // called by the country popup to change the sprites
};