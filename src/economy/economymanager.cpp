#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/controlmanager.h"
#include "bahnhof/economy/entity.h"
#include "bahnhof/economy/money.h"
#include "bahnhof/economy/economymanager.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/stockmarket.h"



EconomyManager::EconomyManager(Game* whatgame) :
	game(whatgame),
	thepublic()
{
	using Economy::Company;
	using Economy::Person;
	using Economy::Portfolio;
	using Economy::ControlMode;

	Person* me = new Person("Sir Charles Darwin", 500_Fr, true);
	entities.emplace_back(me);

    auto& controlmanager = game->getcontrolmanager();
	controlmanager.addcontrolmode(me->generatecontrolmode());
	Portfolio& myinvestments = me->getinvestments();

	Company* sbb = new Company{"SBB AG", stockmarket, 
		{{&myinvestments, 100_Fr},
		 {&thepublic.getinvestments(), 500_Fr}}};
	entities.emplace_back(sbb);

	Company* bls = new Company{"BLS AG", stockmarket, 
		{{&myinvestments, 100_Fr},
		 {&sbb->getinvestments(), 100_Fr},
		 {&thepublic.getinvestments(), 200_Fr}}, true};
	entities.emplace_back(bls);
	ControlMode companycontrol = bls->generatecontrolmode();
	controlmanager.addcontrolmode(companycontrol);
	controlmanager.switchcontrolto(1);

	entities.emplace_back(
		new Company{"Appenzeller Bahnen AG", stockmarket,
		{{&sbb->getinvestments(), 100_Fr}}}
	);
}

void EconomyManager::update(int ms)
{
	stockmarket.update(ms);
}
