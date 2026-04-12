#include "PlayerStrategies.h"
#include "../cards/Cards.h"
#include "../game-engine/GameEngine.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cout, std::cin, std::endl, std::string, std::vector,
    std::stringstream;

// ============================================================
//  ostream operator for PlayerStrategy
// ============================================================
std::ostream &operator<<(std::ostream &os, const PlayerStrategy &ps) {
  os << "Strategy: " << ps.getStrategyName();
  return os;
}

// ============================================================
//  HumanPlayerStrategy
// ============================================================
string HumanPlayerStrategy::getStrategyName() const { return "Human"; }

vector<Territory *> HumanPlayerStrategy::toDefend(const Player *player) const {
    return player->getTerritories();
}

vector<Territory *> HumanPlayerStrategy::toAttack(const Player *player) const {
    int territoriesN = player->mapLoader->getTerritoriesNum();
    vector<bool> arr(territoriesN, false);
    for (auto territory : player->getTerritories()) {
        int territoryId_a = *territory->id;
        for (auto territoryId_b :
            *player->mapLoader->getTerritoryNeighborsIds(territoryId_a)) {
            if (player->mapLoader->getTerritoryPlayerId(*territoryId_b) != player->getId()) {
                arr[*territoryId_b] = true;
            }
        }
    }

    vector<Territory*> list;

    for (int i = 0; i < territoriesN; i++) {
        if (arr[i]) {
            list.push_back((*player->mapLoader->map->territories)[i]);
        }
    }

    return list;
}

void HumanPlayerStrategy::issueOrder(Player *player) {

  cout << "\n--- " << player->getName() << "'s turn (Human) ---" << endl;
  cout << "Reinforcement pool: " << player->getReinforcementPool() << endl;

  // Show territories to defend
  auto defendList = toDefend(player);
  cout << "\nYour territories (defend):" << endl;
  for (Territory *t : defendList) {
    cout << "  id=" << *t->id << " " << *t->getName()
         << " (armies: " << t->getArmiesNum() << ")" << endl;
  }

  // Show territories to attack
  auto attackList = toAttack(player);
  cout << "\nEnemy neighbors (attack):" << endl;
  for (Territory *t : attackList) {
    cout << "  id=" << *t->id << " " << *t->getName()
         << " (armies: " << t->getArmiesNum() << ")" << endl;
  }

  // Show available commands
  cout << "\nAvailable orders:" << endl;
  cout << "  deploy <armNum> <territoryId>" << endl;
  cout << "  advance <armNum> <sourceTerritoryId> <targetTerritoryId>" << endl;
  cout << "  airlift <armNum> <sourceTerritoryId> <targetTerritoryId>" << endl;
  cout << "  bomb <targetTerritoryId>" << endl;
  cout << "  blockade <targetTerritoryId>" << endl;
  cout << "  negotiate <opponentPlayerId>" << endl;
  cout << "\nEnter order: ";

  string input;
  getline(cin, input);

  if (!input.empty()) {
    player->issueOrder(input);
  }
}

// ============================================================
//  AggressivePlayerStrategy
// ============================================================
string AggressivePlayerStrategy::getStrategyName() const {
return "Aggressive";
}

vector<Territory *> AggressivePlayerStrategy::toDefend(const Player *player) const {
  vector<Territory *> defendTerritories = player->toDefend();
  sort(defendTerritories.begin(), defendTerritories.end(),
       [](Territory *a, Territory *b) {
         return a->getArmiesNum() > b->getArmiesNum();
       });
  return defendTerritories;
}

vector<Territory *> AggressivePlayerStrategy::toAttack(const Player *player) const {
  vector<Territory *> defendTerritories = player->toDefend();
  vector<Territory *> attackTerritories;
  for (Territory *t : defendTerritories) {
    for (int *neighborId : *t->getNeighborsIds()) {
      Territory *neighbor = (*player->mapLoader->map->territories)[*neighborId - 1];
      if (neighbor->getPlayerId() != player->getId()) {
        attackTerritories.push_back(neighbor);
      }
    }
  }
  return attackTerritories;
}

void AggressivePlayerStrategy::issueOrder(Player *player) {
  vector<Territory *> defendTerritories = toDefend(player);
  vector<Territory *> attackTerritories = toAttack(player);

  if (defendTerritories.empty())
    return;

  Territory *strongestTerritory = defendTerritories[0];

  // Deploy order: deploy to strongest territory
  player->getOrders()->addOrder(new DeployOrder(
      player, player->getReinforcementPool(), strongestTerritory));

  // Advance order: attack from strongest territory to every adjacent enemy
  // territory
  vector<Territory *> enemyNeighbors;
  for (int *neighborId : *strongestTerritory->getNeighborsIds()) {
    Territory *neighbor = nullptr;
    for (Territory *t : *player->mapLoader->map->territories) {
      if (*t->id == *neighborId) {
        neighbor = t;
        break;
      }
    }
    //if (neighbor && neighbor->getPlayerId() != player->getId()) {
    //  enemyNeighbors.push_back(neighbor);
    //}
  }

  int totalArmies =
      strongestTerritory->getArmiesNum() + player->getReinforcementPool();

  if (totalArmies == 0) return;

  if (enemyNeighbors.empty()) {
    vector<Territory*> friendlyNeighbors;
    for (int *neighborId : *strongestTerritory->getNeighborsIds()) {
      Territory *neighbor = (*player->mapLoader->map->territories)[*neighborId - 1];
      //if (neighbor->getPlayerId() == player->getId()) {
      //  friendlyNeighbors.push_back(neighbor);
      //}
    }
    if (!friendlyNeighbors.empty()) {
      player->getOrders()->addOrder(new AdvanceOrder(
          player, totalArmies, strongestTerritory, friendlyNeighbors[0]));
    }
    return;
  }
  int armiesPerTarget = totalArmies / enemyNeighbors.size();
  int remainder = totalArmies % enemyNeighbors.size();

  for (size_t i = 0; i < enemyNeighbors.size(); i++) {
    int armies = armiesPerTarget + (i == 0 ? remainder : 0);
    if (armies > 0) {
      player->getOrders()->addOrder(new AdvanceOrder(
          player, armies, strongestTerritory, enemyNeighbors[i]));
    }
  }
}

// ============================================================
//  BenevolentPlayerStrategy
// ============================================================
// Computer player that focuses on protecting its weakest country (deploys or advances armies
// on its weakest country, never advances to enemy territories).

string BenevolentPlayerStrategy::getStrategyName() const {
  return "Benevolent";
}

vector<Territory *> BenevolentPlayerStrategy::toDefend(const Player *player) const {
// Strategy is to deploy or advance armies on weakest country

    Territory* weakest = nullptr;
    int lowestArmyCount = *player->getTerritories()[0]->armiesNum;

    // Determine weakest territory
    for (Territory* territory : player->getTerritories()) {
        if (territory->getArmiesNum() < lowestArmyCount) {
            lowestArmyCount = territory->getArmiesNum();
            weakest = territory;
        }
    }

    vector <Territory*> list;
    list.push_back(weakest);

    return list;
}

vector<Territory *> BenevolentPlayerStrategy::toAttack(const Player *player) const {
// Strategy is to never advance to enemy territories
// Therefore, we return an empty list.
    vector <Territory*> list;
    return list;
}

void BenevolentPlayerStrategy::issueOrder(Player *player) {
// Note that there is no attacks needed to be made with this strategy.

    vector<Territory*> defendTerritories = toDefend(player);

    if (defendTerritories.empty())
        return;

    Territory* weakestTerritory = defendTerritories[0];

    // Deploy order: deploy to strongest territory
    player->getOrders()->addOrder(new DeployOrder(
        player, player->getReinforcementPool(), weakestTerritory));
}

// ============================================================
//  NeutralPlayerStrategy
// ============================================================
string NeutralPlayerStrategy::getStrategyName() const { return "Neutral"; }

vector<Territory *> NeutralPlayerStrategy::toDefend(const Player *player) const {
  return player->toDefend();
}

vector<Territory *> NeutralPlayerStrategy::toAttack(const Player *player) const {
  // Neutral player never attacks
  return {};
}

void NeutralPlayerStrategy::issueOrder(Player *player) {
  // Neutral player never issues orders
  cout << player->getName() << " (Neutral) does not issue any orders." << endl;
}

// ============================================================
//  CheaterPlayerStrategy
// ============================================================
// Computer player that automatically conquers all territories that are adjacent to its own
// territories (only once per turn).

string CheaterPlayerStrategy::getStrategyName() const { return "Cheater"; }

vector<Territory *> CheaterPlayerStrategy::toDefend(const Player *player) const {
// Does nothing. 
    vector <Territory*> list;
    return list;
}

vector<Territory*> CheaterPlayerStrategy::toAttack(const Player* player) const {
// Will attack all territories adjacent to its own territories.

    vector<Territory*> list;

    for (Territory* playerTerritory : player->getTerritories()) {
        int playerTerrID = *playerTerritory->id;

        for (auto territoryID : *player->mapLoader->getTerritoryNeighborsIds(playerTerrID)) {
            // If the neighbouring territory does not belong to the player, add to list
            if (player->mapLoader->getTerritoryPlayerId(*territoryID) != player->getId()) {
                list.push_back((*player->mapLoader->map->territories)[*territoryID]);
            }
        }
    }

    return list;
}

void CheaterPlayerStrategy::issueOrder(Player * player) {
// Note that no defending happens with this strategy.

    vector<Territory*> attackTerritories = toAttack(player);

    if (attackTerritories.empty())
        return;

    Territory* startingTerritory = attackTerritories[0];

  // Advance order: attack from strongest territory to every adjacent enemy
  // territory
    //vector<Territory*> enemyNeighbors;
    //for (int* neighborId : *startingTerritory->getNeighborsIds()) {

    //    Territory* neighbor = nullptr;
    //    for (Territory* t : *player->mapLoader->map->territories) {
    //        if (*t->id == *neighborId) {
    //            neighbor = t;
    //            break;
    //        }
    //    }
    //    if (neighbor && neighbor->getPlayerId() != player->getId()) {
    //      enemyNeighbors.push_back(neighbor);
    //    }
    //}

    //int totalArmies =
    //    startingTerritory->getArmiesNum() + player->getReinforcementPool();

    //if (totalArmies == 0) return;

    //if (enemyNeighbors.empty()) {
    //    vector<Territory*> friendlyNeighbors;
    //    for (int* neighborId : *startingTerritory->getNeighborsIds()) {
    //        Territory* neighbor = (*player->mapLoader->map->territories)[*neighborId - 1];
    //        if (neighbor->getPlayerId() == player->getId()) {
    //          friendlyNeighbors.push_back(neighbor);
    //        }
    //    }
    //    if (!friendlyNeighbors.empty()) {
    //        player->getOrders()->addOrder(new AdvanceOrder(
    //            player, totalArmies, strongestTerritory, friendlyNeighbors[0]));
    //    }
    //    return;
    //}
    //int armiesPerTarget = totalArmies / enemyNeighbors.size();
    //int remainder = totalArmies % enemyNeighbors.size();

    //for (size_t i = 0; i < enemyNeighbors.size(); i++) {
    //    int armies = armiesPerTarget + (i == 0 ? remainder : 0);
    //    if (armies > 0) {
    //        player->getOrders()->addOrder(new AdvanceOrder(
    //            player, armies, strongestTerritory, enemyNeighbors[i]));
    //    }
    //}

    // do smth
}
