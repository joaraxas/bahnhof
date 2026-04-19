#pragma once
#include "bahnhof/economy/control.h"

class Building;

namespace Economy{

class Account;
class Portfolio;

struct ControlMode
{
    Entity* entity = nullptr;
    Account* account = nullptr;
    Portfolio* portfolio = nullptr;
    Control<Building>* buildings = nullptr;
};

} // namespace Economy