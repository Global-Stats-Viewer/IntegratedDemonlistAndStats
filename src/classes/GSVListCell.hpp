#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

//geode namespace
using namespace geode::prelude;

class GSVListCell : public CCLayer {
protected:
    matjson::Value m_data; //user data

    virtual bool init();
public:
    static GSVListCell* create(matjson::Value data);
    virtual ~GSVListCell();
};