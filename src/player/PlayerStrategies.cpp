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
      if (player->mapLoader->getTerritoryPlayerId(*territoryId_b) !=
          player->getId()) {
        arr[*territoryId_b] = true;
      }
    }
  }

  vector<Territory *> list;

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

vector<Territory *>
AggressivePlayerStrategy::toDefend(const Player *player) const {
  vector<Territory *> defendTerritories = player->getTerritories();
  sort(defendTerritories.begin(), defendTerritories.end(),
       [](Territory *a, Territory *b) {
         return a->getArmiesNum() > b->getArmiesNum();
       });
  return defendTerritories;
}

vector<Territory *>
AggressivePlayerStrategy::toAttack(const Player *player) const {
  vector<Territory *> defendTerritories = player->toDefend();
  Territory *strongestTerritory = defendTerritories[0];
  vector<Territory *> attackTerritories;
  for (int *neighborId : *strongestTerritory->getNeighborsIds()) {
    if (player->mapLoader->getTerritoryPlayerId(*neighborId) !=
        player->getId()) {
      attackTerritories.push_back(
          (*player->mapLoader->map->territories)[*neighborId]);
    }
  }
  return attackTerritories;
}

void AggressivePlayerStrategy::issueOrder(Player *player) {
  vector<Territory *> defendTerritories = toDefend(player);

  if (defendTerritories.empty())
    return;

  Territory *strongestTerritory = defendTerritories[0];
  vector<Territory *> attackTerritories = toAttack(player);

  if (attackTerritories.empty())
    return;

  // Deploy order: deploy to strongest territory
  player->getOrders()->addOrder(new DeployOrder(
      player, player->getReinforcementPool(), strongestTerritory));

  // Attack with all armies
  int totalArmies = strongestTerritory->getArmiesNum() + player->getReinforcementPool();
  int numTargets = attackTerritories.size();
  int armiesToAdvance = totalArmies / numTargets;
  if (armiesToAdvance < 1) armiesToAdvance = 1;

  for (Territory *target : attackTerritories) {
    player->getOrders()->addOrder(new AdvanceOrder(
        player, armiesToAdvance, strongestTerritory, target));
  }
}

// ============================================================
//  BenevolentPlayerStrategy
// ============================================================
// Computer player that focuses on protecting its weakest country (deploys or
// advances armies on its weakest country, never advances to enemy territories).

string BenevolentPlayerStrategy::getStrategyName() const {
  return "Benevolent";
}

vector<Territory *>
BenevolentPlayerStrategy::toDefend(const Player *player) const {
  // Strategy is to deploy or advance armies on weakest country

  Territory *weakest = player->getTerritories()[0];
  int lowestArmyCount = *player->getTerritories()[0]->armiesNum;

  // Determine weakest territory as the one with the lowest army count
  for (Territory *territory : player->getTerritories()) {
    if (territory->getArmiesNum() < lowestArmyCount) {
      lowestArmyCount = territory->getArmiesNum();
      weakest = territory;
    }
  }

  vector<Territory *> list;
  list.push_back(weakest);

  return list;
}

vector<Territory *>
BenevolentPlayerStrategy::toAttack(const Player *player) const {
  // Strategy is to never advance to enemy territories
  // Therefore, we return an empty list.
  vector<Territory *> list;
  return list;
}

void BenevolentPlayerStrategy::issueOrder(Player *player) {
  // Note that there is no attacks needed to be made with this strategy.

  vector<Territory *> defendTerritories = toDefend(player);

  if (defendTerritories.empty())
    return;

  Territory *weakestTerritory = defendTerritories[0];

  // Deploy order: deploy to weakest territory
  player->getOrders()->addOrder(new DeployOrder(
      player, player->getReinforcementPool(), weakestTerritory));
}

// ============================================================
//  NeutralPlayerStrategy
// ============================================================
string NeutralPlayerStrategy::getStrategyName() const { return "Neutral"; }

vector<Territory *>
NeutralPlayerStrategy::toDefend(const Player *player) const {
  return player->getTerritories();
}

vector<Territory *>
NeutralPlayerStrategy::toAttack(const Player *player) const {
  // Neutral player never attacks
  return {};
}

void NeutralPlayerStrategy::issueOrder(Player *player) {
  // Neutral player never issues orders
}

// ============================================================
//  CheaterPlayerStrategy
// ============================================================
// Computer player that automatically conquers all territories that are adjacent
// to its own territories (only once per turn).

string CheaterPlayerStrategy::getStrategyName() const { return "Cheater"; }

vector<Territory *>
CheaterPlayerStrategy::toDefend(const Player *player) const {
 // Gets all the player territories that are adjacent to the enemy territories
// Used for the attack in issueOrder()
  vector<Territory *> list;

  for (Territory *playerTerritory : player->getTerritories()) {
    int playerTerrID = *playerTerritory->id;

    for (auto territoryID :
         *player->mapLoader->getTerritoryNeighborsIds(playerTerrID)) {
      // If the neighbouring territory belongs to the player, add the player
      // territory to list
      if (player->mapLoader->getTerritoryPlayerId(*territoryID) !=
          player->getId()) {
        list.push_back(playerTerritory);
      }
    }
  }

  return list;
}

vector<Territory *>
CheaterPlayerStrategy::toAttack(const Player *player) const {
  // Will attack all territories adjacent to its own territories.

  vector<Territory *> list;

  for (Territory *playerTerritory : player->getTerritories()) {
    int playerTerrID = *playerTerritory->id;

    for (auto territoryID :
         *player->mapLoader->getTerritoryNeighborsIds(playerTerrID)) {
      // If the neighbouring territory does not belong to the player, add to
      // list
      if (player->mapLoader->getTerritoryPlayerId(*territoryID) !=
          player->getId()) {
        list.push_back((*player->mapLoader->map->territories)[*territoryID]);
      }
    }
  }

  return list;
}

void CheaterPlayerStrategy::issueOrder(Player *player) {
  // Note that no defending happens with this strategy.

  vector<Territory *> attackTerritories = toAttack(player);
  vector<Territory *> defendTerritories = toDefend(player);

  if (attackTerritories.empty())
    return;

  if (defendTerritories.empty())
    return;

  for (int i = 0; i < attackTerritories.size(); i++) {
    int totalArmies =
        defendTerritories[i]->getArmiesNum() + player->getReinforcementPool();
    // Cheats are on, so in AdvanceOrder it will bypass to auto conquering (see
    // AdvanceOrder::execute)
    player->getOrders()->addOrder(new AdvanceOrder(
        player, totalArmies, defendTerritories[i], attackTerritories[i], true));
  }
}
