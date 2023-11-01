#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>


using namespace std;

StarterBot::StarterBot()
{

}

// Called when the bot starts!
void StarterBot::onStart()
{
   openingBuildOrderString = readOpeningBuildOrderFromConfig();
   //transfer(openingBuildOrderString);

    std::cout << "Opening build order:";
    for (int h = 0; h < openingBuildOrderString.size(); h++)
    {
        std::cout << ' ' << openingBuildOrderString[h];
    }
    std::cout << "\n";

    //BWAPI::Broodwar->printf("ordrcehck %s", nextOrder.c_str());

    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(5);
    BWAPI::Broodwar->setFrameSkip(0);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
    transfer(openingBuildOrderString);

}

// Called on each frame of the game
void StarterBot::onFrame()
{
    

    //***************************************************************
        // Update our MapTools information

    supplyUsed = BWAPI::Broodwar->self()->supplyUsed() / 2;
    supplyTotal = BWAPI::Broodwar->self()->supplyTotal() / 2;
    workersOwned = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Terran_SCV); //monitor the worker we have
    mineralOwned = BWAPI::Broodwar->self()->minerals();
    gasOwned = BWAPI::Broodwar->self()->gas();
    barracksNum = Tools::CountMyUnitsOfType(barracks);
    bunkeramt = Tools::CountMyUnitsOfType(bunker);

    m_mapTools.onFrame();
    orderManagement();

    if (bunkeramt <= 2 && base2 != nullptr) {bunkerRise();}
    

    if (barracksNum >= 1 && openingBuildOrder.empty()) { trainBarrack(marine); buildAdditionalSupply();}
    if (enemyFound && mineralOwned >= BWAPI::Broodwar->self()->getRace().getResourceDepot().mineralPrice() && expand != playerBase && base2 == nullptr)
    {
        expansionSecond();
    }
    
    if (m_squad.size() >= 0) { attack(); }




    scout();
    harassment();

    BWAPI::Broodwar->drawCircleMap(enemyBase, 32, BWAPI::Colors::Red, true);
    if (groupPos) {
        BWAPI::Broodwar->drawCircleMap(setUpPos, 32, BWAPI::Colors::Red, true);
    }

    BWAPI::Broodwar->drawCircleMap(setUpPos, 20, BWAPI::Colors::Blue, true);

    BWAPI::Broodwar->drawCircleMap(bunkerPos, 32, BWAPI::Colors::Red, true);
    sendIdleWorkersToMinerals();

    sendIdleWorkersToRefineries();
    //underAttack();
    Tools::DrawUnitHealthBars();
    drawDebugInformation();


}

void StarterBot::transfer(std::vector<std::string> order)
{
    // take element from the order and find the unit type
    // then push the related unit type in the openingBuildOrder
    if (openingBuildOrder.size() == openingBuildOrderString.size()) 
    { 
        //BWAPI::Broodwar->printf("%d", openingBuildOrder.size());
        return; 
    }
    for (int j = 0; j < order.size(); j++)
    {
        if (order[j] == "Terran_SCV")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_SCV);
        }
        else if (order[j] == "Terran_Marine")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Marine);
        }
        else if (order[j] == "Terran_Bunker")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Bunker);
        }
        else if (order[j] == "Terran_Refinery")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Refinery);
        }
        else if (order[j] == "Terran_Barracks")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Barracks);
        }
        else if (order[j] == "Terran_Supply_Depot")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Supply_Depot);
        }
        else if (order[j] == "Terran_Factory")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Factory);
        }
        else if (order[j] == "Terran_Academy")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Academy);
        }
        else if (order[j] == "Terran_Armory")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Armory);
        }
        else if (order[j] == "Terran_Engineering_Bay")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Engineering_Bay);
        }
        else if (order[j] == "Terran_Missile_Turret")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Missile_Turret);
        }
        else if (order[j] == "Terran_Starport")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Starport);
        }
        else if (order[j] == "Terran_Science_Facility")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Science_Facility);
        }
        else if (order[j] == "Terran_Comsat_Station")
        {
            openingBuildOrder.push_back(BWAPI::UnitTypes::Terran_Comsat_Station);
        }

    }
    
}
std::string remove_quotes(const std::string& input) {
    std::string output;
    for (char c : input) {
        if (c != '\"') output += c;
    }
    return output;
}
//make a function that reads content from a txt file called "StarterBot_Config.txt"
std::vector<std::string> StarterBot::readOpeningBuildOrderFromConfig()
{
    std::vector<std::string> openingBuildOrderString;

    // Open the config file
    std::ifstream configFile("../bin/StarterBot_Config.txt");

    // Check if file was opened successfully
    if (!configFile)
    {
        std::cerr << "Error: Unable to open config file." << std::endl;
        return openingBuildOrderString;
    }

    // Open the output file
    std::ofstream outFile("OpeningBuildOrder.txt");

    // Check if file was opened successfully
    if (!outFile)
    {
        std::cerr << "Error: Unable to open output file." << std::endl;
        configFile.close();
        return openingBuildOrderString;
    }

    // Read the file line by line
    std::string line;
    while (std::getline(configFile, line))
    {
        // Check if the line contains the opening build order
        if (line.find("\"OpeningBuildOrder\"") != std::string::npos)
        {
            // Find the start of the opening build order array
            std::size_t start = line.find("[");
            if (start == std::string::npos) { break; }

            // Find the end of the opening build order array
            std::size_t end = line.find("]");
            if (end == std::string::npos) { break; }

            // Extract the opening build order from the line
            std::string buildOrder = line.substr(start + 1, end - start - 1);

            // Tokenize the opening build order into individual elements
            std::istringstream iss(buildOrder);
            std::string element;
            while (std::getline(iss, element, ','))
            {
                // Remove leading and trailing whitespace from the element
                element.erase(0, element.find_first_not_of(" \t\n\r\f\v"));
                element.erase(element.find_last_not_of(" \t\n\r\f\v") + 1);
                element = remove_quotes(element);
                // Add the element to the opening build order vector
                openingBuildOrderString.push_back(element);
            }

            break;
        }
    }

    // Write the opening build order to the output file
    for (const auto& element : openingBuildOrderString)
    {
        outFile << element << std::endl;
    }

    // Close the files
    configFile.close();
    outFile.close();

    return openingBuildOrderString;
}
//***************************************
bool StarterBot::checkOrder(BWAPI::UnitType jj, BWAPI::Unit builder)
{
    print("2222222222222222222");
    const BWAPI::UnitCommand& command = builder->getLastCommand();
    if (command.getType() == jj)
    {
        return true;
    }
    else if (command.getType() == BWAPI::UnitCommandTypes::Gather)
    {
        print("rethink@@rethink@@rethink@@rethink@@rethink@@");
    }
    return false;

}



bool StarterBot::allBuildingsConstructed()
{
    for (const auto& entry : buildingTypeCount)
    {
        BWAPI::UnitType type = entry.first;
        int count = entry.second;

        int constructedCount = Tools::CountUnitsOfType(type, BWAPI::Broodwar->self()->getUnits());
        BWAPI::Broodwar->printf("%d", constructedCount);
        if (constructedCount < count)
        {
            buildingType = type;
            return false;
        }
    }
    return true;
}
bool StarterBot::checkBuilderState(BWAPI::Unit builder, BWAPI::UnitType jj)
{
    if(!checkOrder(jj,builder))
    {
        int countjj = Tools::CountUnitsOfType(jj, BWAPI::Broodwar->self()->getUnits());
        if (countjj < buildingTypeCount[jj])
        {
            buildingTypeCount[jj]--;
            redflag = false;
            complete = false;
            return false;
		}
        else if (countjj == buildingTypeCount[jj]) { complete = true; redflag = false; return true; }
    }
}

bool StarterBot::getOrder(BWAPI::UnitType type)
{
    if (type.isBuilding())
    {
        if (mineralOwned >= type.mineralPrice() + 50) {return build(type, start);}
        else { return false; }
            
    }
    else if (type.isWorker() && mineralOwned >= 50)
    {

        return trainWorkers();
    }
    return false;
}

void StarterBot::orderManagement()
{
    if (!openingBuildOrder.empty())
    {
        BWAPI::UnitType nextOrder = openingBuildOrder.front();


        if (!getOrder(nextOrder))
        {
            return;
        }
        // Add the order to the current orders
        else
        {
            openingBuildOrder.pop_front();

            currentOrder.push_back(nextOrder);
            // BWAPI::Broodwar->printf("current siez %d", currentOrder.size());
        }
    }

    else
    {
        return;
    }
}


BWAPI::Unit StarterBot::getClosestEnemyBattleUnit(BWAPI::Unit unit)
{
    if (currentTarget && currentTarget->exists() && currentTarget->getHitPoints() > 0)
    {
        return currentTarget;
    }

    BWAPI::Unit closestCombatUnit = nullptr;
    int closestCombatDistance = INT_MAX;
    BWAPI::Unit closestWorkerUnit = nullptr;
    int closestWorkerDistance = INT_MAX;
    BWAPI::Unit closestBuildingUnit = nullptr;
    int closestBuildingDistance = INT_MAX;

    const BWAPI::Unitset& EnemyUnits = BWAPI::Broodwar->enemy()->getUnits();

    // Prioritize enemy combat units
    for (auto& enemyUnit : EnemyUnits)
    {
        if (enemyUnit->isVisible() && !enemyUnit->getType().isBuilding() && !enemyUnit->getType().isWorker())
        {
            int distance = unit->getDistance(enemyUnit);
            if (distance < closestCombatDistance)
            {
                closestCombatUnit = enemyUnit;
                closestCombatDistance = distance;
            }
        }
    }

    if (closestCombatUnit)
    {
        currentTarget = closestCombatUnit;
        return closestCombatUnit;
    }

    // If no combat units, prioritize enemy workers
    for (auto& enemyUnit : EnemyUnits)
    {
        if (enemyUnit->isVisible() && enemyUnit->getType().isWorker())
        {
            int distance = unit->getDistance(enemyUnit);
            if (distance < closestWorkerDistance)
            {
                closestWorkerUnit = enemyUnit;
                closestWorkerDistance = distance;
            }
        }
    }

    if (closestWorkerUnit)
    {
        currentTarget = closestWorkerUnit;
        return closestWorkerUnit;
    }

    // If no combat units and workers, prioritize enemy buildings
    for (auto& enemyUnit : EnemyUnits)
    {
        if (enemyUnit->isVisible() && enemyUnit->getType().isBuilding())
        {
            int distance = unit->getDistance(enemyUnit);
            if (distance < closestBuildingDistance)
            {
                closestBuildingUnit = enemyUnit;
                closestBuildingDistance = distance;
            }
        }
    }

    currentTarget = closestBuildingUnit;
    return closestBuildingUnit;
}

bool StarterBot::isBlocking(BWAPI::Unit unit)
{

    for (auto& worker : BWAPI::Broodwar->self()->getUnits())
    {
        if (worker->getType().isWorker() && worker->getDistance(unit) < 32)
        {
            return true;
        }
    }

    return false;
}



void StarterBot::underAttack()
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->isUnderAttack() && unit->getDistance(playerBase) >= 1000) // if unit is under attack and unit is away from base
        {
            return;
        }
        else if (unit->isUnderAttack() && unit->getDistance(playerBase) <= 200) // if unit is under attack in base territory
        {
            BWAPI::Position gotInvade = unit->getPosition();
            if ((unit->getType().isBuilding() || unit->getType().isWorker()) && unit != m_scout && unit->getDistance(playerBase) <= 1000)
            {
                for (auto& other : myUnits)
                {
                    if (isCombatUnit(other->getType()) && other->getDistance(playerBase) <= 1000)
                    {
                        for (auto& invader : BWAPI::Broodwar->enemy()->getUnits())
                        {
                            other->attack(invader);
                        }
                    }
                    else if (isCombatUnit(other->getType()) && other->getDistance(playerBase) > 1000)
                    {
                        if (other->getType().isWorker())
                        {
                            for (auto& invader : BWAPI::Broodwar->enemy()->getUnits())
                            {
                                other->attack(invader);
                            }

                        }
                    }
                }
            }
        }
    }
}
void StarterBot::attack()
{
    for (auto& unit : m_squad)
    {
        if (!unit->exists()) 
        { 
            auto it = std::find(m_squad.begin(), m_squad.end(), unit);
            if (it != m_squad.end())
            {
                m_squad.erase(it);
            }
            return;
        }
        // Check the unit type, if it is a combat unit, send it to attack
        if (isCombatUnit(unit->getType()) && !unit->isLoaded()) // if this is a combat unit
        {
            if (warList.size() > 0 && !unit->isAttacking() && unit->isUnderAttack())
            {
                BWAPI::Unit target = unit->getTarget();
                auto tar = std::find(warList.begin(), warList.end(),target);
                if (tar != warList.end()) { return; }
                BWAPI::Unit closestEnemy = nullptr;
                int closestDistance = 250;

                // Loop through the warList to find the closest enemy unit
                for (auto& enemy : warList)
                {
                    // If the enemy unit exists and is visible
                    if (enemy->exists() && enemy->isVisible())
                    {
                        // Calculate the distance between the unit and the enemy
                        int distance = unit->getDistance(enemy);

                        // If the distance is closer than the current closest distance
                        if (distance < closestDistance)
                        {
                            // Update the closest enemy unit and the closest distance
                            closestEnemy = enemy;
                            closestDistance = distance;
                        }
                    }
                }
                if (closestEnemy != nullptr)
                {
                    unit->attack(closestEnemy);

                }
            }
            if (unit->getDistance(playerBase) < 1000 && !unit->isUnderAttack()) // if unit is at player base
            {
                    unit->attack(enemyBase); // attack the enemy base
            }
            else if (unit->getDistance(playerBase) > 1000 && unit->isIdle() && !unit->isUnderAttack()) // if unit is outside player base and not doing anything
            {
                if (enemyBuildings.size()>=0) // if there are enemy units
                {
                    BWAPI::Broodwar->printf("Attack next enemy!");
                    unit->attack(enemyBuildings.front());

                }
                else // if no enemy buildings in sight, search for one
                {
                    // generate random coordinates within 1500 radius of the enemy base
                    int x = rand() % 3000 - 1500;
                    int y = rand() % 3000 - 1500;
                    BWAPI::Position pos = BWAPI::Position(enemyBase.x + x, enemyBase.y + y).makeValid();
                    BWAPI::Broodwar->printf("Finding next enemy at %d, %d", pos.x, pos.y);
                    // scout around and attack anything on its way to new position
                    unit->attack(pos);

                }
            }
        }
    }

}

bool StarterBot::trainBarrack(BWAPI::UnitType type)
{
    // Check if the player has enough resources to train the unit
    if (BWAPI::Broodwar->self()->minerals() >= type.mineralPrice() &&
        BWAPI::Broodwar->self()->gas() >= type.gasPrice() )
    {
        // Train the unit at the randomly selected Barracks
        Idlebarracks = IdleBarracks();
        if (Idlebarracks != nullptr)
        {
            return Idlebarracks->train(type);
        }
    }
    return false;
}
BWAPI::Unit StarterBot::IdleFactorys()
{


    // Get all of the player's Barracks units
    std::vector<BWAPI::Unit> factoryList;

    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Factory && !unit->isTraining() && unit->isCompleted())
        {
            factoryList.push_back(unit);

        }
    }

    // If there are no idle Barracks, do nothing
    if (factoryList.empty())
    {
        return nullptr;
    }
    int randomFactorys = rand() % factoryList.size();
    return factoryList[randomFactorys];

}
void StarterBot::trainFactory(BWAPI::UnitType type)
{
    // Check if the player has enough resources to train the unit
    if (mineralOwned >= type.mineralPrice() &&
        gasOwned >= type.gasPrice())
    {
        // Train the unit at the randomly selected Barracks
        Idlefactorys = IdleFactorys();
        if (Idlefactorys != nullptr)
        {
            Idlefactorys->train(type);
            return;
        }
        return;
    }

}
void StarterBot::checkScoutTargeted()
{

    if (m_scout == nullptr || !m_scout->exists() )
    {
        return;
    }
    istargetted = false;
    // Check if m_scout is being targeted
    if (m_scout->getDistance(playerBase) >= 60 && enemyFound)
    {
        if(m_scout->isUnderAttack()) { istargetted = true; return; }
        for (auto& unit : BWAPI::Broodwar->enemy()->getUnits())
        {
            if (unit->getTarget() == m_scout && !unit->getType().isBuilding())
            {
                istargetted = true;
                return; // Break out of the loop when a targeting enemy unit is found
            }
        }
    }
}

void StarterBot::harassment()
{
        if (!enemyFound) { return; }
        if (!m_scout || !m_scout->exists() || m_scout == nullptr)
        {
            return;
        }
        checkScoutTargeted();
        if (istargetted && m_scout->getDistance(base1) < 60)
        {
            istargetted = false;
            return;
        }
        else if (istargetted && m_scout->getDistance(base1)>=100)
        {
            //BWAPI::Position retreatDirection = playerBase - m_scout->getPosition();
            //retreatDirection = retreatDirection / retreatDirection.getLength() * 100; // Normalize the direction and multiply by a scalar
            //BWAPI::Position retreatPosition = m_scout->getPosition() + retreatDirection;
            if (m_scout == nullptr) { return; }
            m_scout->move(playerBase);
            return;
        }
        else
        {
            // Check if the scout is attacking
            if (m_scout == nullptr) { return; }
            else if (m_scout->isAttacking())
            {
                if (m_scout == nullptr) { return; }
                m_scout->move(m_scout->getPosition());
                return;
            }
            else
            {
                // Find an enemy building to attack
                BWAPI::Unit enemyBuildingToAttack = m_scout->getClosestUnit(BWAPI::Filter::IsEnemy);

                // If an enemy building is found, attack it
                if (m_scout == nullptr) { return; }
                else if (enemyBuildingToAttack != nullptr)
                {
                    if (m_scout == nullptr) { return; }
                    m_scout->attack(enemyBuildingToAttack);
                    return;
                }
                else
                {
                    if (m_scout == nullptr) { return; }
                    // If no enemy buildings are found, move the scout to the enemy base
                    m_scout->move(enemyBase);
                    return;
                }
            }
        }

}
void StarterBot::scout()
{

    if (enemyFound) { return; }
    if (m_scout == nullptr) { 
        return;
    }

    if (enemyFound == false)
    {
        // if we find an enemy unit, mark the location and return scout to base.
        BWAPI::Unit pos_enemy = m_scout->getClosestUnit();
        if (BWAPI::Broodwar->self()->isEnemy(pos_enemy->getPlayer())) // if closest unit is an enemy
        {
            for (auto& unit : BWAPI::Broodwar->enemy()->getUnits())
            {
                if (unit->getType().isBuilding() && unit->getType().isResourceDepot()) // if enemy unit is a resource depot
                {
                    enemyBase = unit->getPosition(); // set enemy base as enemy unit's position
                    enemyFound = true;
                    BWAPI::Broodwar->printf("Enemy Found: %d, %d", enemyBase.x, enemyBase.y);

                    return;
                }
                else {
                    enemyFound = false;
                }
            }
        }
        if (!groupPos)
        {
            int distance = m_scout->getDistance(playerBase);
            if (distance >= 870)
            {
                setUpPos = m_scout->getPosition(); // set chokepoint to the position of the scout
                BWAPI::Broodwar->printf("setUpPos x: %d,setUpPos y: %d", setUpPos.x, setUpPos.y);
                groupPos = true;
            }
        }
        if (!bunkerPosition)
        {
            int distance = m_scout->getDistance(playerBase);
            if (distance >= 850)
            {
                bunkerPos = m_scout->getPosition(); // set bunker position to the position of the scout
                BWAPI::Broodwar->printf("bunkerPos x: %d,bunkerPos y: %d", bunkerPos.x, bunkerPos.y);
                bunkerPosition = true;
            }

        }

        // code for sending the scout around the map.
        auto& startLocations = BWAPI::Broodwar->getStartLocations();
        for (BWAPI::TilePosition tp : startLocations) // for each start location
        {
            BWAPI::TilePosition tp1 = BWAPI::TilePosition(tp.x + 5, tp.y + 5);
            BWAPI::TilePosition tp2 = BWAPI::TilePosition(tp.x - 5, tp.y + 5);
            BWAPI::TilePosition tp3 = BWAPI::TilePosition(tp.x + 5, tp.y - 5);
            BWAPI::TilePosition tp4 = BWAPI::TilePosition(tp.x - 5, tp.y - 5);
            std::deque<BWAPI::TilePosition> surroundings = { tp, tp1, tp2, tp3, tp4 }; // add all the surrounding tiles to the deque
            for (BWAPI::TilePosition tile : surroundings)
            {
                if (!BWAPI::Broodwar->isExplored(tile))
                {
                    BWAPI::Position pos(tile);

                    auto command = m_scout->getLastCommand();
                    if (command.getTargetPosition() == pos) // if scout is going to this tile, do nothing
                    {
                        return;
                    }
                    m_scout->move(pos); // else, move scout to this tile
                    return;
                }
            }
        }

    }
}


void StarterBot::bunkerRise()
{
    if (base2 == nullptr || !base2->isCompleted()) { return; }
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() == BWAPI::UnitCommandTypes::Build)
        {

            BWAPI::UnitType buildingType = command.getUnitType();
            if (buildingType == BWAPI::UnitTypes::Terran_Bunker)
            {
                return;
            }
        }


    }
    BWAPI::TilePosition desiredPos = BWAPI::TilePosition(bunkerPos);

    int maxBuildRange = 12;
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Bunker, desiredPos, maxBuildRange, false);
    if (mineralOwned >= BWAPI::UnitTypes::Terran_Bunker.mineralPrice())
    {
        build(BWAPI::UnitTypes::Terran_Bunker, buildPos);
    }


}



void StarterBot::expansionSecond()
{
    if ( !openingBuildOrder.empty() )
    {
        return;
    }
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand& command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() == BWAPI::UnitCommandTypes::Build)
        {

            BWAPI::UnitType buildingType = command.getUnitType();
            if (buildingType.isResourceDepot())
            {
                return;
            }
        }
    }

    BWAPI::TilePosition desiredPos = BWAPI::TilePosition(expand);

    int maxBuildRange = 1;
    int count = 1;
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(BWAPI::UnitTypes::Terran_Bunker, desiredPos, maxBuildRange, false);
    if (mineralOwned >= BWAPI::UnitTypes::Terran_Command_Center.mineralPrice())
    {
        count++;
        build(BWAPI::UnitTypes::Terran_Command_Center, buildPos);
    }


}






void StarterBot::mineralAround(BWAPI::Unit unit, std::set<int>ID, std::deque<BWAPI::Unit>rocks)
{

    const BWAPI::Unitset& rock = BWAPI::Broodwar->getMinerals();
    for (auto& units : rock)
    {
        int distance = units->getDistance(unit);
        if ((ID.find(units->getID()) == ID.end()) && distance <= 150) {
            ID.insert(units->getID());
            rocks.push_back(units);
        }
        else if ((ID.find(units->getID()) == ID.end()) && distance <= 1000)
        {
            expansionMinerals1ID.insert(units->getID());
            expansionMinerals1.push_back(units);

        }
    }

    detected = true;
}



// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    //const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    //std::map<BWAPI::Unit, int> mineralsWorkersMap;
    //for (auto& unit : myUnits)
    //{
    //    // Check the unit type, if it is an idle worker, then we want to send it somewhere
    //    if (unit->getType().isWorker() && unit->isIdle() && unit != m_exp && unit->isCompleted())
    //    {
    //        // Get the closest mineral to this worker unit
    //        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());
    //        if (closestMineral && mineralsWorkersMap[closestMineral] < 2)
    //        {
    //            unit->rightClick(closestMineral);
    //            mineralsWorkersMap[closestMineral]++;
    //            outOfResource = false;

    //        }
    //        else
    //        {
    //            outOfResource = true;
    //        }
    //    }
    //}

        if (refineryFilled || outOfResource || gasWorkersMap.empty())
        {
            const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
            for (auto& unit : myUnits)
            {
                if (unit->getType().isWorker() && unit->isIdle() && !unit->isConstructing() && unit != m_scout && unit->exists())
                {
                    BWAPI::Unit ava = nullptr;
                    int closestDist = std::numeric_limits<int>::max();
                    for (const auto& map : mineralsWorkersMap)
                    {
                        if (map.first->getDistance(base1) <= 200)
                        {
                            if (map.second < 3 && map.first->exists())
                            {
                                int dist = unit->getDistance(map.first);
                                if (dist < closestDist)
                                {
                                    ava = map.first;
                                    closestDist = dist;
                                }
                            }
                        }
                    }

                    if (ava)
                    {
                        unit->rightClick(ava);
                        mineralsWorkersMap[ava]++;
                    }
                    else { outOfResource = true; return; }
                }
            }
        }
    
}

void StarterBot::expansion()
{
    if (!enemyFound) { return; }

    const BWAPI::Unitset& rock = BWAPI::Broodwar->getMinerals();
    BWAPI::Broodwar->printf("%d", expansionMinerals1.size());
    for (auto& units : rock)
    {
        int distance = units->getDistance(units);
        if ((mineralsID.find(units->getID()) == mineralsID.end()) && distance <= 150) {
            mineralsID.insert(units->getID());
            minerals.push_back(units);
        }
        else if ((mineralsID.find(units->getID()) == mineralsID.end()) && (expansionMinerals1ID.find(units->getID()) == expansionMinerals1ID.end()) && distance >= 150 && distance <= 700)
        {
            expansionMinerals1ID.insert(units->getID());
            expansionMinerals1.push_back(units);
        }
        else if ((mineralsID.find(units->getID()) == mineralsID.end()) && (expansionMinerals1ID.find(units->getID()) == expansionMinerals1ID.end()) && (expansionMinerals2ID.find(units->getID()) == expansionMinerals2ID.end()) && distance <= 1000)
        {
            expansionMinerals2ID.insert(units->getID());
            expansionMinerals2.push_back(units);
        }
    }

    //if (enemyBase && mineralOwned >= Tools::GetDepot()->getType().mineralPrice() && expansionMinerals1.size() >=1)
    //{
           // build(Tools::GetDepot()->getType(), m_builder, 2, expansionMinerals1[0]->getTilePosition());
    //}
    //if (enemyBase && mineralOwned >= Tools::GetDepot()->getType().mineralPrice() && expansionMinerals2.size() >= 1)
   // {
        //BWAPI::Unit inva = m_builder;
        //inva->move(expansionMinerals2[0]->getPosition());
        //build(Tools::GetDepot()->getType(), inva, 3, expansionMinerals2[0]->getTilePosition());
    //}

}
// 

void StarterBot::sendIdleWorkersToRefineries()
{
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    if (gasWorkersMap.empty() || refineryFilled ) 
    {
        return;
    }
    for (auto& count : gasWorkersMap)
    {
        if (count.second < 3) { refineryFilled = false; }
    }
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        if (unit->getType().isWorker() && unit->isIdle() && !unit->isConstructing() && unit != m_scout && unit->exists())
        {
            BWAPI::Unit closestGas = nullptr;
            int closestDist = std::numeric_limits<int>::max();  // set to maximum possible int value

            for (const auto& map : gasWorkersMap)
            {
                if (map.second < 4 && map.first->exists())
                {
                    int dist = unit->getDistance(map.first);
                    if (dist < closestDist)
                    {
                        closestGas = map.first;
                        closestDist = dist;
                    }
                }
            }
            if (closestGas)
            {
                unit->rightClick(closestGas);
                gasWorkersMap[closestGas]++;
            }
            else if (closestGas == nullptr) { refineryFilled = true; }
            
        }
    }
}

// Train more workers so we can gather more income

bool StarterBot::trainWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    const int workersWanted = workersOwned + 1;
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { return myDepot->train(workerType); }
        else { return false; }
    }
    
}
void StarterBot::trainAdditionalWorkers()
{

    BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    while (workers <= 23)
    {
        const BWAPI::Unit myDepot = Tools::GetDepot();

        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
        workers++;

    }
}







// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Bonan Yin\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{

    if (unit->getPlayer() == BWAPI::Broodwar->enemy() && unit->getType().isBuilding())
    {
        for (unsigned int i = 0; i < enemyBuildings.size(); i++)
        {
            if (enemyBuildings[i] == unit)
            {
                BWAPI::Broodwar->printf("Enemy building removed");
                enemyBuildings.erase(enemyBuildings.begin() + i);
                break;
            }
        }
    }

    if (unit == m_scout)
    {
        m_scout = nullptr; 
    }
    if (unit->getType().isMineralField()) { mineralsWorkersMap.erase(unit); }
    if (unit->getType().isRefinery()) { gasWorkersMap.erase(unit); }
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{

}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text)
{
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{

}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
    if (unit->getPlayer() == BWAPI::Broodwar->self() && unit->getType() == BWAPI::UnitTypes::Terran_Bunker)
    {
        bunker1 = unit;
    }
    if (unit->getPlayer() == BWAPI::Broodwar->self() && unit->getType().isResourceDepot() && unit->isCompleted() && unit !=base1)
    {
        base2 = unit;
    }
    if (isCombatUnit(unit->getType()))
    {
        if (setUpPos != playerBase && bunkeramt == 0) {unit->rightClick(setUpPos);}

        if (bunkeramt >= 1 && bunker1 != nullptr && insider <= 4) { unit->rightClick(bunker1); insider++; }
         
    }
    if (unit->getType() == BWAPI::UnitTypes::Terran_Academy) { m_academy = unit; }
    if (unit->getType() == BWAPI::UnitTypes::Terran_Engineering_Bay) { m_engineering = unit; }
    if (unit->getType() == BWAPI::UnitTypes::Terran_SCV)
    {

        if (workersArray.find(unit->getID()) == workersArray.end())
        {
            workersArray.insert(unit->getID());
            workerInarray.push_back(unit);
            workers++;
            if (workersArray.size() == 4)
            {
                m_scout = workerInarray[3];
                BWAPI::Broodwar->printf("scout assigned");

            }

        }
    }
    if (isCombatUnit(unit->getType())) 
    { 
        squadcount++;
        if (squadcount >= 10) 
        {
            const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
            for (auto& unit : myUnits)
            {
                // Check the unit type, if it is a combat unit, send it to attack
                if (isCombatUnit(unit->getType()) && !unit->isLoaded()) // if this is a combat unit
                {
                    m_squad.push_back(unit);
                }
            }
            squadcount = 0; 
        }
    }




}
BWAPI::Unit StarterBot::IdleBarracks()
{
    // Get all of the player's Barracks units
    std::vector<BWAPI::Unit> barracksList;

    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Barracks && !unit->isTraining() && unit->isCompleted())
        {
            barracksList.push_back(unit);
        }
    }
    // If there are no idle Barracks, do nothing
    if (barracksList.empty())
    {
        barracksidle = false;
        return nullptr;
    }
    barracksidle = true;
    int randomBarracks = rand() % barracksList.size();
    return barracksList[randomBarracks];
}



bool StarterBot::isDefensiveBuilding(BWAPI::Unit unit)
{
    BWAPI::UnitType type = unit->getType();
    if (type == BWAPI::UnitTypes::Terran_Bunker || type == BWAPI::UnitTypes::Zerg_Sunken_Colony || type == BWAPI::UnitTypes::Protoss_Photon_Cannon)
    {
        return true;
    }
    return false;
}

bool StarterBot::isCombatUnit(BWAPI::UnitType type)
{
    if (type == BWAPI::UnitTypes::Terran_Marine ||
        type == BWAPI::UnitTypes::Terran_Firebat ||
        type == BWAPI::UnitTypes::Terran_Medic ||
        type == BWAPI::UnitTypes::Terran_Ghost ||
        type == BWAPI::UnitTypes::Terran_Vulture ||
        type == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode ||
        type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode ||
        type == BWAPI::UnitTypes::Terran_Goliath ||
        type == BWAPI::UnitTypes::Terran_Wraith ||
        type == BWAPI::UnitTypes::Terran_Dropship ||
        type == BWAPI::UnitTypes::Terran_Battlecruiser ||
        type == BWAPI::UnitTypes::Terran_Valkyrie ||
        type == BWAPI::UnitTypes::Terran_Science_Vessel || 
        type == BWAPI::UnitTypes::Zerg_Zergling ||
        type == BWAPI::UnitTypes::Zerg_Hydralisk||
        type == BWAPI::UnitTypes::Zerg_Ultralisk||
        type == BWAPI::UnitTypes::Zerg_Mutalisk||
        type == BWAPI::UnitTypes::Zerg_Guardian||
        type == BWAPI::UnitTypes::Zerg_Queen||
        type == BWAPI::UnitTypes::Zerg_Defiler||
        type == BWAPI::UnitTypes::Zerg_Scourge||
        type == BWAPI::UnitTypes::Zerg_Lurker||
        type == BWAPI::UnitTypes::Zerg_Devourer||
        type == BWAPI::UnitTypes::Protoss_Zealot||
        type == BWAPI::UnitTypes::Protoss_Dragoon||
        type == BWAPI::UnitTypes::Protoss_Archon||
        type == BWAPI::UnitTypes::Protoss_Dark_Archon||
        type == BWAPI::UnitTypes::Protoss_Scout||
        type == BWAPI::UnitTypes::Protoss_Arbiter||
        type == BWAPI::UnitTypes::Protoss_Carrier||
        type == BWAPI::UnitTypes::Protoss_Shuttle||
        type == BWAPI::UnitTypes::Protoss_Reaver||
        type == BWAPI::UnitTypes::Protoss_Observer||
        type == BWAPI::UnitTypes::Protoss_High_Templar||
        type == BWAPI::UnitTypes::Protoss_Dark_Templar||
        type == BWAPI::UnitTypes::Protoss_Corsair)
    {
        return true;
    }

    return false;
}


// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{

    if ((unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser || ((unit->getType().isMineralField())) && unit->getInitialResources() > 100) && (unit->getDistance(baseDepot) >= 500 && unit->getDistance(baseDepot)<=1500))
    {
        expand = unit->getPosition();

    }

    if (unit->getPlayer() == BWAPI::Broodwar->enemy() && unit->getType().isBuilding()) // if unit is an enemy building
    {
        BWAPI::Broodwar->printf("Enemy building added");
        enemyBuildings.push_back(unit);
    }
    if (unit->getPlayer() == BWAPI::Broodwar->enemy() && (isDefensiveBuilding(unit) || isCombatUnit(unit->getType()) || unit->getType().isWorker()) && (unit->getDistance(base1) <= 200))
    {
        warList.push_back(unit);
    }

    if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()) && unit->getDistance(playerBase) <= 800)
    {
        gotengage = true;
    }
    if (unit == base2)
    {
        bunkerPos = base2->getPosition();
    }

    if (unit->getType().isMineralField() && (unit->getDistance(base1) <= 200 ||(base2 != nullptr && unit->getDistance(base2) <= 250)))
    {
        mineralsWorkersMap[unit] = 0;
    }
    if (unit->getType().isRefinery() && unit->isCompleted() && ((unit->getDistance(base1) <= 200 || (base2 != nullptr && unit->getDistance(base2) <= 250))))
    {
        gasWorkersMap[unit] = 0;
    }
}

void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 5) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    PreBuildBuilding(supplyProviderType);

}

void StarterBot::print(const std::string& str)
{
    BWAPI::Broodwar->printf("%s", str.c_str());
}
// if the queue is all fullfilled, then the stratege shifts on this function


// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{
    //pop out the unit from our warList
    warList.erase(std::remove(warList.begin(), warList.end(), unit), warList.end());
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{

}
BWAPI::Unit StarterBot::GetUnitOfType(BWAPI::UnitType type)  // copy from the Tool.cpp
{
    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted())
        {
            if (unit->getType().isWorker() && !unit->isGatheringGas() && !unit->isConstructing() && unit!=m_scout && unit->isCompleted()) { return unit; }
            else if (!unit->getType().isWorker()) { return unit; }

        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}
bool StarterBot::build(BWAPI::UnitType type,  BWAPI::TilePosition des) 
{
    BWAPI::UnitType builderType = type.whatBuilds().first;
    BWAPI::Unit builder = GetUnitOfType(builderType);
    if (type.mineralPrice() > mineralOwned || type.gasPrice() > gasOwned || (type.mineralPrice() > mineralOwned && type.gasPrice()))
    {
        return false;
    }
    if((builder == m_scout && m_scout != nullptr) || builder->isConstructing() || !builder->isCompleted()) { return false; }
    if (builder == nullptr) { return false; }
    
    int maxBuildRange = 60;
    bool buildingOnCreep = type.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, des, maxBuildRange, buildingOnCreep);

    while (!buildPos.isValid())
    {
        maxBuildRange = maxBuildRange + 8;
        BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, des, maxBuildRange, buildingOnCreep);
        if (maxBuildRange >= 800) { BWAPI::Broodwar->printf("no valid place");  return false; }
    }
    builder->stop();
    if (builder->build(type, buildPos))
    {
        buildingTypeCount[type]++;
        return true;
    }
    return false;
}

bool StarterBot::DFSBuild(BWAPI::UnitType type, BWAPI::TilePosition start)
{
    std::queue<BWAPI::TilePosition> queue;
    std::set<BWAPI::TilePosition> visited;

    queue.push(start);
    BWAPI::UnitType builderType = type.whatBuilds().first;

    while (!queue.empty())
    {
        BWAPI::TilePosition current = queue.front();
        queue.pop();

        if (visited.count(current) > 0)
            continue;

        visited.insert(current);

        // If the current tile is a valid build location, build there
        if (BWAPI::Broodwar->canBuildHere(current, type))
        {
            // Get a builder and order it to build at this location
            BWAPI::Unit builder = GetUnitOfType(builderType);
            if ((builder == m_scout && m_scout != nullptr) || builder->isConstructing()) { return false; }
            if (builder == nullptr) { return false; }
            if (builder->build(type, current))
            {
                buildingTypeCount[type]++;
                return true;
            }

        }

        // Add adjacent tiles to the queue
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                BWAPI::TilePosition next = current + BWAPI::TilePosition(dx, dy);
                if (next.isValid())
                {
                    queue.push(next);
                }
            }
        }
    }

    // No build location found
    return false;
}
