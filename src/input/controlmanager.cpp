#include "bahnhof/input/controlmanager.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/economy/economymanager.h"

ControlManager::ControlManager(Game* whatgame) : 
    game{whatgame},
    controlmode{}
{
    auto& economymanager = game->getgamestate().geteconomymanager();
    Economy::ControlMode mycontrol = economymanager.me.generatecontrolmode();
	controlmodes.push_back(mycontrol);
	controlmode = mycontrol;
}

const Economy::ControlMode* const ControlManager::getcontrolmode() const
{
    return &controlmode;
}

bool ControlManager::switchcontrolto(int index)
{
    if(index<0 || index>controlmodes.size())
        return false;
    controlmode = controlmodes[index];
    return true;
}