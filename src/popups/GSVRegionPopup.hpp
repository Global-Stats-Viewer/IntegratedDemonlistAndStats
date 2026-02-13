#include <Geode/Geode.hpp>

//geode namespace
using namespace geode::prelude;

class GSVRegionPopup : public Popup {
protected:
	bool init() override;
    virtual ~GSVRegionPopup();

    async::TaskHolder<web::WebResponse> m_listener;

	CCLayer* m_mainLayer;
	TextInput* m_searchInput;
    ListView* m_list;
    CCMenuItemSpriteExtra* m_reset;
    LoadingCircle* m_loadCircle;

    std::string m_countryCode = ""; // Used for subdivisions since they have the country as a prefix in the database
    bool m_useSubdivisions = false;
    matjson::Value m_data;
    bool m_isLoaded = false;

public:
	static GSVRegionPopup* create(bool useSubdivisions, std::string countryCode = "");

    void onSelect(CCObject*);
    void onClear(CCObject*);
    void onUpdate(const std::string&);
};