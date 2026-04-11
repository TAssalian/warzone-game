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

vector<Territory *> HumanPlayerStrategy::toDefend(Player *player) {
  return player->toDefend();
}

vector<Territory *> HumanPlayerStrategy::toAttack(Player *player) {
  return player->toAttack();
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

vector<Territory *> AggressivePlayerStrategy::toDefend(Player *player) {
  vector<Territory *> defendTerritories = player->toDefend();
  sort(defendTerritories.begin(), defendTerritories.end(),
       [](Territory *a, Territory *b) {
         return a->getArmiesNum() > b->getArmiesNum();
       });
  return defendTerritories;
}

vector<Territory *> AggressivePlayerStrategy::toAttack(Player *player) {
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
    if (neighbor && neighbor->getPlayerId() != player->getId()) {
      enemyNeighbors.push_back(neighbor);
    }
  }

  int totalArmies =
      strongestTerritory->getArmiesNum() + player->getReinforcementPool();

  if (totalArmies == 0) return;

  if (enemyNeighbors.empty()) {
    vector<Territory*> friendlyNeighbors;
    for (int *neighborId : *strongestTerritory->getNeighborsIds()) {
      Territory *neighbor = (*player->mapLoader->map->territories)[*neighborId - 1];
      if (neighbor->getPlayerId() == player->getId()) {
        friendlyNeighbors.push_back(neighbor);
      }
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
string BenevolentPlayerStrategy::getStrategyName() const {
  return "Benevolent";
}

vector<Territory *> BenevolentPlayerStrategy::toDefend(Player *player) {}

vector<Territory *> BenevolentPlayerStrategy::toAttack(Player *player) {}

void BenevolentPlayerStrategy::issueOrder(Player *player) {}

// ============================================================
//  NeutralPlayerStrategy
// ============================================================
string NeutralPlayerStrategy::getStrategyName() const { return "Neutral"; }

vector<Territory *> NeutralPlayerStrategy::toDefend(Player *player) {
  return player->toDefend();
}

vector<Territory *> NeutralPlayerStrategy::toAttack(Player *player) {
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
string CheaterPlayerStrategy::getStrategyName() const { return "Cheater"; }

vector<Territory *> CheaterPlayerStrategy::toDefend(Player *player) {}

vector<Territory *> CheaterPlayerStrategy::toAttack(Player *player) {
  return {};
}

void CheaterPlayerStrategy::issueOrder(Player *player) {}
