#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace Economy{
class PlayerControl{
    bool is{false};
public:
    PlayerControl(bool playercontrolled) : is(playercontrolled) {}
    bool operator()() const {return is;}
    void operator()(bool set) {is = set;}
};



class ControllerPointerBase
{
public:
    virtual PlayerControl& getcontrol() const = 0;
    virtual bool operator()() const = 0;
};

class ControllerPointerDirect : public ControllerPointerBase
{
    PlayerControl& control;
public:
    ControllerPointerDirect(PlayerControl& c) : control{c} {}
    PlayerControl& getcontrol() const override {return control;}
    bool operator()() const {return control();}
};

template<typename T>
class ControllerPointer : public ControllerPointerBase
{
    T* control;
public:
    ControllerPointer(T& owner) : control{&owner} {}
    PlayerControl& getcontrol() const override {return control->getplayercontrol();}
    bool operator()() const {auto& pcontrol = getcontrol(); return pcontrol();}
    T& getowner() const {return *control;}
    void setowner(T& newowner) {control = &newowner;}
};

} // end namespace Economy