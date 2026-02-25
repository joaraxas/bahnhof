#pragma once
#include "bahnhof/common/forwardincludes.h"

class InterfaceManager;

class Entity
{
public:
    ~Entity() {}
    virtual const std::string& getname() const = 0;
    virtual void createpanel(InterfaceManager* ui) = 0;
};