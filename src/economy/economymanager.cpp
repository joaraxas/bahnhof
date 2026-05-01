#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/controlmanager.h"
#include "bahnhof/economy/economymanager.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/stockmarket.h"



EconomyManager::EconomyManager(Game* whatgame) :
	me("Sir Charles Darwin", 500, true),
	thepublic(),
	game(whatgame)
{
	using Economy::Company;
	using Economy::Portfolio;
	using Economy::ControlMode;

    auto& controlmanager = game->getcontrolmanager();
    ControlMode mycontrol = me.generatecontrolmode();
	controlmanager.addcontrolmode(mycontrol);
	Portfolio* myinvestments = &me.getinvestments();

	Company* sbb = new Company{"SBB AG", stockmarket, 
		{{myinvestments, 100},
		 {&thepublic.getinvestments(), 500}}};
	companies.emplace_back(sbb);

	Company* bls = new Company{"BLS AG", stockmarket, 
		{{myinvestments, 100},
		 {&sbb->getinvestments(), 100}}, true};
	companies.emplace_back(bls);
	ControlMode companycontrol = bls->generatecontrolmode();
	controlmanager.addcontrolmode(companycontrol);
	controlmanager.switchcontrolto(1);

	companies.emplace_back(
		new Company{"Appenzeller Bahnen AG", stockmarket,
		{{&sbb->getinvestments(), 100}}}
	);
}

void EconomyManager::update(int ms)
{
	stockmarket.update(ms);
}
