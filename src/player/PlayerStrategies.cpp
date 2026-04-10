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

vector<Territory *> HumanPlayerStrategy::toDefend(Player *player) {}

vector<Territory *> HumanPlayerStrategy::toAttack(Player *player) {}

void HumanPlayerStrategy::issueOrder(Player *player) {}

// ============================================================
//  AggressivePlayerStrategy
// ============================================================
string AggressivePlayerStrategy::getStrategyName() const {
  return "Aggressive";
}

vector<Territory *> AggressivePlayerStrategy::toDefend(Player *player) {}

vector<Territory *> AggressivePlayerStrategy::toAttack(Player *player) {}

void AggressivePlayerStrategy::issueOrder(Player *player) {}

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

vector<Territory *> NeutralPlayerStrategy::toDefend(Player *player) {}

vector<Territory *> NeutralPlayerStrategy::toAttack(Player *player) {}

void NeutralPlayerStrategy::issueOrder(Player *player) {}

// ============================================================
//  CheaterPlayerStrategy
// ============================================================
string CheaterPlayerStrategy::getStrategyName() const { return "Cheater"; }

vector<Territory *> CheaterPlayerStrategy::toDefend(Player *player) {}

vector<Territory *> CheaterPlayerStrategy::toAttack(Player *player) {

  void CheaterPlayerStrategy::issueOrder(Player * player) {}
