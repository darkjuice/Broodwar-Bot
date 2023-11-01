#pragma once

#include "MapTools.h"
#include "Tools.h"
#include <set>
#include <BWAPI.h>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <ctime>   // for the time() function
#include <cstdlib> // for the srand() function

class StarterBot
{
    MapTools m_mapTools;
	int supplyUsed = BWAPI::Broodwar->self()->supplyUsed() / 2;
	int supplyTotal = BWAPI::Broodwar->self()->supplyTotal() / 2;
	int workersOwned = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_SCV); //monitor the worker we have
	int mineralOwned = BWAPI::Broodwar->self()->minerals();
	int gasOwned = BWAPI::Broodwar->self()->gas();
	int barracksNum = Tools::CountMyUnitsOfType(barracks);
	int factoryNum = Tools::CountMyUnitsOfType(factorys);
	int bunkeramt = Tools::CountMyUnitsOfType(bunker);


	bool onTraining = false ;
	bool onBuilding = false;
	bool onRefinery = false;
	bool tainingComplete = false;
	bool enemyFound = false;
	bool enemyBaseDestroyed = false;
	bool bunkerFilled = false;
	bool outOfResource = false;

	bool bunkerFilled2 = false;
	bool expPos = false;
	bool findbuilder;
	bool machineShop = false;
	bool barracksidle;
	bool barracksnew = false;
	bool factorynew = false;
	bool factoryidle;
	bool idleBuilders;
	bool complete = false;
	int workersCur = 4;

	int squadEngage = 10;
	int wave = 0;

	int supplyPrice = BWAPI::UnitTypes::Terran_Supply_Depot.supplyProvided();
	int barracksPrice = BWAPI::UnitTypes::Terran_Barracks.mineralPrice();
	int engineeringPrice = BWAPI::UnitTypes::Terran_Engineering_Bay.mineralPrice();
	int academyPrice = BWAPI::UnitTypes::Terran_Academy.mineralPrice();
	int bunkerPrice = BWAPI::UnitTypes::Terran_Bunker.mineralPrice();
	int refineryPrice = BWAPI::UnitTypes::Terran_Refinery.mineralPrice();
	int factoryMinPrice = BWAPI::UnitTypes::Terran_Factory.mineralPrice();
	int factoryGasPrice = BWAPI::UnitTypes::Terran_Factory;

	int marinePrice = BWAPI::UnitTypes::Terran_Marine.mineralPrice();
	int medicMinPrice = BWAPI::UnitTypes::Terran_Medic.mineralPrice();
	int medicGasPrice = BWAPI::UnitTypes::Terran_Medic.gasPrice();
	int firebatMinPrice = BWAPI::UnitTypes::Terran_Firebat.mineralPrice();
	int firebatGasPrice = BWAPI::UnitTypes::Terran_Firebat.gasPrice();

	int vulturePrice = BWAPI::UnitTypes::Terran_Vulture.mineralPrice();
	int tankMinPrice = BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.mineralPrice();
	int tankGasPrice = BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.gasPrice();
	const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
	BWAPI::Position enemyBase;

	BWAPI::Position playerBase = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
	BWAPI::Position bunkerPos;
	BWAPI::Position setUpPos = playerBase;;
	BWAPI::Position gotInvaded;
	BWAPI::Unit baseDepot = Tools::GetDepot();
	BWAPI::Unit m_scout = nullptr;
	BWAPI::Unit builder1 = nullptr;
	BWAPI::Unit builder2 = nullptr;
	BWAPI::Unit builder3 = nullptr;
	BWAPI::Unit builder4 = nullptr;
	BWAPI::Unit m_builder = nullptr;

	BWAPI::Unit m_gas1 = nullptr;
	BWAPI::Unit m_gas2 = nullptr;
	BWAPI::Unit m_gas3 = nullptr;
	BWAPI::Unit m_exp = nullptr;
	BWAPI::Unit barracks1 = nullptr;
	BWAPI::Unit barracks2 = nullptr;
	BWAPI::Unit barracks3 = nullptr;
	BWAPI::Unit factory1 = nullptr;
	BWAPI::Unit factory2 = nullptr;
	BWAPI::Unit bunker1 = nullptr;
	BWAPI::Unit bunker2 = nullptr;
	BWAPI::Unit bunker3 = nullptr;
	BWAPI::Unit m_engineering = nullptr;
	BWAPI::Unit m_academy = nullptr;
	BWAPI::Unit m_machine = nullptr;

	BWAPI::Unit refinerydone = nullptr;
	BWAPI::Unit Idlebarracks = nullptr;
	BWAPI::Unit Idlefactorys = nullptr;
	BWAPI::Unit m_refinery = nullptr;

	BWAPI::Unit m_last = nullptr;;
	BWAPI::Unit expansionBase1 = nullptr;
	BWAPI::Unit expansionBase2 = nullptr;
	BWAPI::Unit base1 = Tools::GetDepot();
	BWAPI::Unit base2 = nullptr;
	BWAPI::Unit currentTarget = nullptr;
	BWAPI::Position expand = baseDepot->getPosition();
	BWAPI::Position expand2 = playerBase;

	BWAPI::UnitType workersType = BWAPI::Broodwar->self()->getRace().getWorker();
	BWAPI::UnitType barracks = BWAPI::UnitTypes::Terran_Barracks;
	BWAPI::UnitType bunker = BWAPI::UnitTypes::Terran_Bunker;
	BWAPI::UnitType factorys = BWAPI::UnitTypes::Terran_Factory;
	BWAPI::UnitType academy = BWAPI::UnitTypes::Terran_Academy;
	BWAPI::UnitType machine = BWAPI::UnitTypes::Terran_Machine_Shop;
	BWAPI::UnitType engineering = BWAPI::UnitTypes::Terran_Engineering_Bay;
	BWAPI::UnitType supply = BWAPI::UnitTypes::Terran_Supply_Depot;
	BWAPI::UnitType refinery = BWAPI::UnitTypes::Terran_Refinery;
	BWAPI::UnitType marine = BWAPI::UnitTypes::Terran_Marine;
	BWAPI::UnitType medic = BWAPI::UnitTypes::Terran_Medic;
	BWAPI::UnitType firebat = BWAPI::UnitTypes::Terran_Firebat;
	BWAPI::UnitType vulture = BWAPI::UnitTypes::Terran_Vulture;
	BWAPI::UnitType tank = BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode;

	BWAPI::UnitType buildingType;
	

	BWAPI::TilePosition start = BWAPI::Broodwar->self()->getStartLocation();
	BWAPI::Position newsetUpPos = baseDepot->getPosition();


	int cur;

	int workers = 0;
	int supplydepot = 0;
	int numOfConstrutingField;
	int construct=0;
	int insider = 0;;


	int academyNum ;
	int engineeringNum ;
	int machineNum;
	int marineNum;
	int medicNum;
	int firebatNum;
	int vultureNum;
	int tankNum;
	int field = 0;
	int count = 0;
	int index = 0;
	int road;
	int squadcount = 0;
	int wavecount = 0;

	bool refineryFilled = false;;
	bool constructing ;
	bool detected = false;
	bool gasdetected= false;
	bool attacking = false;
	bool groupPos = false;
	bool istargetted = false;

	bool bunkerPosition = false;
	bool gotengage = false;
	bool redflag = false;
	

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();
	std::vector<std::string> readOpeningBuildOrderFromConfig();
    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
	void orderManagement();

	bool trainBarrack(BWAPI::UnitType type );
	void trainFactory(BWAPI::UnitType type);
	void scout();
	void alwaysOnWatch();
	void bunkerRise();
	void assignMarinesToBunker();

	void sendIdleWorkersToRefineries();
	bool isCombatUnit(BWAPI::UnitType type);
	void attack();
	void underAttack();
	void takeOver();
	void orderReconnect();
	bool checkOrder(BWAPI::UnitType jj, BWAPI::Unit builder);
	void transfer(std::vector<std::string> order);
	bool trainWorkers();
	bool getOrder(BWAPI::UnitType type);
	void vaccantBuilder();
	
	BWAPI::Unit IdleFactorys();
	bool build(BWAPI::UnitType type, BWAPI::TilePosition des);
	bool isReserved(const BWAPI::TilePosition& pos);
	void reservePosition(const BWAPI::TilePosition& pos);
	void sendIdleWorkers();
	void mineralAround(BWAPI::Unit unit, std::set<int>ID, std::deque<BWAPI::Unit>rock);
	void expansion();
	void explore();
	void getAllCombatUnit();
	bool isBlocking(BWAPI::Unit unit);
	void expansionSecond();
	void print(const std::string& str);
	void checkScoutTargeted();
	bool allBuildingsConstructed();
	bool PreBuildBuilding(BWAPI::UnitType type);
	BWAPI::Unit IdleBarracks();
	BWAPI::Position randomInRange(int x, int y);
	BWAPI::Unitset enemyBattleUnit();
	BWAPI::Unit getNextAvailableMineralPatch(std::map<BWAPI::Unit, int> map, std::deque<BWAPI::Unit>rock);
	BWAPI::Unit getClosestEnemyBattleUnit(BWAPI::Unit unit);
	BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);

	void harassment();
	bool DFSBuild(BWAPI::UnitType type, BWAPI::TilePosition start);
	void exploreTerritory();
	bool isDefensiveBuilding(BWAPI::Unit unit);
	bool checkBuilderState(BWAPI::Unit builder,BWAPI::UnitType jj);
	bool ReissueBuilding(BWAPI::Unit builder);



	std::deque<BWAPI::UnitType> openingBuildOrder;
	std::deque<BWAPI::UnitType> currentOrder;
	std::deque<BWAPI::Unit> enemyBuildings; 
	std::deque<BWAPI::Unit> myBuildings;
	std::deque<BWAPI::Unit> m_squad;
	std::deque<BWAPI::Unit> bunkerunits;
	std::deque<BWAPI::Unit> refineryworker;
	std::deque<BWAPI::Unit> builders;
	std::deque<BWAPI::Unit> warList;


	BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();
	BWAPI::UnitType enemyBaseType = BWAPI::Broodwar->enemy()->getRace().getResourceDepot();
	std::vector<std::string>openingBuildOrderString;
	//std::vector<std::string>testorder = { "Terran_SCV", "Terran_SCV", "Terran_SCV", "Terran_SCV", "Terran_Barracks", "Terran_SCV" , "Terran_SCV" ,"Terran_Supply_Depot" ,"Terran_SCV" , "Terran_SCV" , "Terran_SCV","Terran_Refinery", "Terran_SCV","Terran_Factory", "Terran_SCV", "Terran_SCV", "Terran_Academy" };
	std::set<int> workersArray;
	std::deque<BWAPI::Unit> workerInarray;
	std::deque<BWAPI::Unit> barracksArray;
	std::deque<BWAPI::Unit> squad;
	std::set<int>barracksID;
	std::set<int> factoryID;
	std::vector<BWAPI::Unit> factoryArray;
	std::vector<BWAPI::TilePosition> newpos;

	
	std::set<int> mineralsID;
	std::deque<BWAPI::Unit>minerals;
	std::set<int> expansionMinerals1ID;
	std::deque<BWAPI::Unit>expansionMinerals1;
	std::set<int> expansionMinerals2ID;
	std::deque<BWAPI::Unit>expansionMinerals2;
	std::set<int> squadID;



	std::map<BWAPI::UnitType, int> buildingTypeCount;
	std::map<BWAPI::Unit, int> gasWorkersMap;
	std::map<BWAPI::Unit, int> mineralsWorkersMap;



};