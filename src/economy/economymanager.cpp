#include<map>
#include "bahnhof/economy/economymanager.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/person.h"
#include "bahnhof/economy/thepublic.h"
#include "bahnhof/economy/stockmarket.h"



EconomyManager::EconomyManager(Game* whatgame) :
	me("Sir Charles Darwin", 500, true),
	thepublic()
{
	using Economy::Company;
	game = whatgame;
	Company* bls = new Company{"BLS AG", stockmarket};
	companies.emplace_back(bls);
	if(!bls->getcompanysshares().issue(100, me.getinvestments()))
		throw "couldn't emit BLS shares";
	Company* sbb = new Company{"SBB AG", stockmarket};
	companies.emplace_back(sbb);
	if(!sbb->getcompanysshares().issue(50, bls->getcompanysinvestments()))
		throw "couldn't emit SBB shares";
	sbb->getcompanysshares().issue(500, thepublic.getinvestments());
	me.getinvestments().buy(bls->getcompanysinvestments(), sbb->getcompanysshares(), 5);
	companies.emplace_back(new Company{"Appenzeller Bahnen AG", stockmarket});
}

void EconomyManager::update(int ms)
{
	stockmarket.update(ms);
}
