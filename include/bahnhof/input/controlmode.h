#pragma once
#include "bahnhof/economy/control.h"

class Building;

namespace Economy{

class Account;
class Portfolio;

struct ControlMode
{
    std::string identifier;
    Account* account = nullptr;
    Portfolio* portfolio = nullptr;
    Control<Building>* buildings = nullptr;
};

} // namespace Economy