#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "controlmode.h"

class Game;

class ControlManager
{
public:
    ControlManager(Game* whatgame);
    const Economy::ControlMode* const getcontrolmode() const;
    const std::vector<Economy::ControlMode>& getavailablecontrolmodes() const {return controlmodes;};
    void addcontrolmode(Economy::ControlMode mode) {controlmodes.push_back(mode);};
    bool switchcontrolto(int index);
private:
    Game* game;
    Economy::ControlMode controlmode;
    std::vector<Economy::ControlMode> controlmodes;
};
