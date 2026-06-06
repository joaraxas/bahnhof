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
}