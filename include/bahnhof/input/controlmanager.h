#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "controlmode.h"

class Game;

// Handles the pool of available control modes to allow a player to switch between roles of action.
class ControlManager
{
public:
    ControlManager(Game* whatgame);
    const Economy::ControlMode& getcontrolmode() const;
    const std::vector<Economy::ControlMode>& getavailablecontrolmodes() const {return controlmodes;};
    void addcontrolmode(Economy::ControlMode mode) {controlmodes.push_back(mode);};
    bool switchcontrolto(int index);
private:
    Game* game;
    Economy::ControlMode controlmode;
    std::vector<Economy::ControlMode> controlmodes;
};
