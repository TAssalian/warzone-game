#include "Orders.h"
#include "../cards/Cards.h"
#include "../player/Player.h"
#include <algorithm>
using namespace std;

GameEngine* Order::gameEngine = nullptr;

//-------------ORDER IMPLEMENTATION--------------//
// Default Constructor
Order::Order()
    : orderType(""), isExecuted(false), orderEffect(""), issuer(nullptr) {}

// Parameterized Constructor
Order::Order(std::string orderType, Player *player)
    : orderType(orderType), isExecuted(false), orderEffect(""), issuer(player) {
}

// Getters
std::string Order::getOrderType() const { return orderType; }

Player *Order::getPlayer() const { return issuer; }

bool Order::getIsExecuted() const { return isExecuted; }

std::string Order::getOrderEffect() const { return orderEffect; }

// Setters
void Order::setOrderType(const std::string orderType) {
  this->orderType = orderType;
}

void Order::setPlayer(Player *player) { this->issuer = player; }

void Order::setIsExecuted(const bool isExecuted) {
  this->isExecuted = isExecuted;
}

void Order::setEffect(const std::string orderEffect) {
  this->orderEffect = orderEffect;
}

// Methods

bool Order::validate() { return true; }

bool Order::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Order has been executed.");
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Order validation failed. Order has not been executed.");
    return false;
  }
}

std::ostream &operator<<(std::ostream &os, const Order &order) {
  os << "Order Type: " << order.getOrderType() << ", Issuer: "
     << (order.getPlayer() ? order.getPlayer()->getName() : "None")
     << ", Is Executed: " << (order.getIsExecuted() ? "Yes" : "No")
     << ", Effect: " << order.getOrderEffect();
  return os;
}

//-------------DEPLOY ORDER IMPLEMENTATION--------------//
// Default Constructor
DeployOrder::DeployOrder()
    : Order("Deploy", nullptr), numArmies(0), targetTerritory(nullptr) {}

// Parameterized Constructor
DeployOrder::DeployOrder(Player *issuer, int numArmies,
                         Territory *targetTerritory)
    : Order("Deploy", issuer), numArmies(numArmies),
      targetTerritory(targetTerritory) {}

// Getters
int DeployOrder::getNumArmies() const { return numArmies; }

Territory *DeployOrder::getTargetTerritory() const { return targetTerritory; }

// Setters

void DeployOrder::setArmies(const int numArmies) {
  this->numArmies = numArmies;
}

void DeployOrder::setTerritory(Territory *targetTerritory) {
  this->targetTerritory = targetTerritory;
}

// Methods
bool DeployOrder::validate() {
  // Check if target territory is null
  if (targetTerritory == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect(
        "Deploy order validation failed. Target territory or issuer is null.");
    return false;
  }

  return true;

  // Check if the target territory belongs to the player that issued the order
  const vector<Territory *> playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory == targetTerritory) {
      if (issuer->getReinforcementPool() < numArmies) {
        setIsExecuted(false);
        setEffect("Deploy order validation failed. Not enough armies in the "
                  "reinforcement pool.");
        return false;
      }
      return true;
    }
  }
  setIsExecuted(false);
  setEffect("Deploy order validation failed. Target territory does not belong "
            "to the issuer.");
  return false;
}

bool DeployOrder::execute() {
  if (validate()) {

    // Add armies to the target territory
    targetTerritory->setArmiesNum(targetTerritory->getArmiesNum() +
                                  numArmies);
    // Deduct from reinforcement pool
    // issuer->setReinforcementPool(issuer->getReinforcementPool() - numArmies);
    setIsExecuted(true);
    setEffect("Deployed " + to_string(numArmies) + " armies to " +
              *(targetTerritory->getName()));
    return true;
  }
  return false;
}

//-------------ADVANCE ORDER IMPLEMENTATION--------------//
// Default Constructor
AdvanceOrder::AdvanceOrder()
    : Order("Advance", nullptr), numArmies(0), source(nullptr),
      target(nullptr) {}

// Parameterized Constructor
AdvanceOrder::AdvanceOrder(Player *issuer, int numArmies, Territory *source,
                           Territory *target)
    : Order("Advance", issuer), numArmies(numArmies), source(source),
      target(target) {}

// Getters
int AdvanceOrder::getNumArmies() const { return numArmies; }

Territory *AdvanceOrder::getSourceTerritory() { return source; }

Territory *AdvanceOrder::getTargetTerritory() { return target; }

// Setters
void AdvanceOrder::setNumArmies(const int numArmies) {
  this->numArmies = numArmies;
}

void AdvanceOrder::setSourceTerritory(Territory *source) {
  this->source = source;
}

void AdvanceOrder::setTargetTerritory(Territory *target) {
  this->target = target;
}

// Methods

bool AdvanceOrder::validate() {
  // Check if source/target/issuer is null
  if (source == nullptr || target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Source/target territory or "
              "issuer is null.");
    return false;
  }

  // Check if the source territory belongs to the player that issued the order
  const vector<Territory *> playerTerritories = issuer->getTerritories();
  bool sourceOwned = false;
  for (Territory *territory : playerTerritories) {
    if (territory == source) {
      sourceOwned = true;
      break;
    }
  }
  if (!sourceOwned) {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Source territory does not "
              "belong to the issuer.");
    return false;
  }
  // Check if the players are in peace
  for (Player *negotiatedPlayer : issuer->getNegotiatedPlayers()) {
    if (negotiatedPlayer->getId() == *(target->playerId)) {
      setIsExecuted(false);
      setEffect("Advance order validation failed. Issuer is in peace with "
                "target player.");
      return false;
    }
  }

  //Check if the source territory has at least numArmies armies and numArmies is higher than 0
  if (numArmies > source->getArmiesNum() || numArmies < 0) {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Issued armiesNum should be between 1 to sourceTerritory armiesNum");
    return false;
  }

  // Check if the target territory is adjacent to the source territory
  vector<int *> *neighbourIds = source->getNeighborsIds();
  for (int *neighbourId : *neighbourIds) {
    if (*neighbourId == *target->id) {
      return true;
    }
  }
  setIsExecuted(false);
  setEffect("Advance order validation failed. Target territory is not adjacent "
            "to source territory.");

  return false;
}

bool AdvanceOrder::execute() {
  if (validate()) {
    // If the target is owned by the same player, just move armies
    if (*(target->playerId) == *(source->playerId)) {
      source->setArmiesNum(source->getArmiesNum() - numArmies);
      target->setArmiesNum(target->getArmiesNum() + numArmies);
      setIsExecuted(true);
      setEffect("Moved " + to_string(numArmies) + " armies from " +
                *(source->getName()) + " to " + *(target->getName()));
    } else {
      // Check if there is a negotiate peace treaty in effect
      // Find the defender player by scanning issuer's negotiated list
      if (issuer->isNegotiatedWith(nullptr)) {
        // placeholder; real check is done per-player below
      }
      // We can't directly look up the Player* from playerId here without a
      // game-level registry, so we track negotiation on the issuer side.
      // The NegotiateOrder::execute() stores the target Player* on issuer.
      // Advanced check: compare target->playerId against negotiated players'
      // ids. For now, trust that NegotiateOrder sets up bidirectional entries.

      // Deduct attacking armies from source
      source->setArmiesNum(source->getArmiesNum() - numArmies);

      int attackingArmies = numArmies;
      int defendingArmies = target->getArmiesNum();

      // Battle simulation
      while (attackingArmies > 0 && defendingArmies > 0) {
        // Each attacker has 60% chance to kill a defender
        if ((rand() % 100) < 60) {
          defendingArmies--;
        }
        // Each defender has 70% chance to kill an attacker
        if ((rand() % 100) < 70) {
          attackingArmies--;
        }
      }

      if (defendingArmies > 0) {
        // Defender wins — restore surviving defenders
        target->setArmiesNum(defendingArmies);
        setIsExecuted(true);
        setEffect("Attacked " + *(target->getName()) + " and lost.");
      } else {
        // Attacker wins — capture territory
        target->setArmiesNum(attackingArmies);
        
        // if target player is not neutral, remove the target territory from the players territories list
        if (*target->playerId != -1) {
          gameEngine->getPlayers()[*target->playerId]->removeTerritory(target);
        }  
        
        
        *target->playerId = *source->playerId; // transfer ownership
        issuer->addTerritory(target);
        issuer->setConqueredThisTurn(true);
        setIsExecuted(true);
        setEffect("Attacked " + *(target->getName()) + " and conquered it.");
      }
    }
    return true;
  }
  return false;
}

//-------------BOMB ORDER IMPLEMENTATION--------------//

// Default Constructor
BombOrder::BombOrder() : Order("Bomb", nullptr), target(nullptr) {}

// Parameterized Constructor
BombOrder::BombOrder(Player *issuer, Territory *target)
    : Order("Bomb", issuer), target(target) {}

// Getters
Territory *BombOrder::getTargetTerritory() const { return target; }

// Setters
void BombOrder::setTargetTerritory(Territory *target) { this->target = target; }

// Methods
bool BombOrder::validate() {
  // Check if target territory or issuer is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect(
        "Bomb order validation failed. Target territory or issuer is null.");
    return false;
  }

  // Target must NOT belong to the issuer
  const vector<Territory *> playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory == target) {
      setIsExecuted(false);
      setEffect("Bomb order validation failed. Target territory belongs to the "
                "issuer.");
      return false;
    }
  }
  // Check if the players are in peace
  for (Player *negotiatedPlayer : issuer->getNegotiatedPlayers()) {
    if (negotiatedPlayer->getId() == *(target->playerId)) {
      setIsExecuted(false);
      setEffect("Bomb order validation failed. Issuer is in peace with target "
                "player.");
      return false;
    }
  }

  // Target must be adjacent to at least one of the issuer's territories
  bool isAdjacent = false;
  for (Territory *territory : playerTerritories) {
    vector<int *> *neighbourIds = territory->getNeighborsIds();
    for (int *neighbourId : *neighbourIds) {
      if (*neighbourId == *target->id) {
        isAdjacent = true;
        break;
      }
    }
  }

  if (!isAdjacent) {
    setIsExecuted(false);
    setEffect("Bomb order validation failed. Target territory is not adjacent "
              "to any of the issuer territories.");
    return false;
  }

  // Check if the player has a Bomb card
  bool hasCard = false;
  for (Card *card : *issuer->getHand()->cards) {
    if (card->type == CardType::Bomb) {
      hasCard = true;
      break;
    }
  }
  if (!hasCard) {
    setIsExecuted(false);
    setEffect(
        "Bomb order validation failed. Issuer does not have a Bomb card.");
    return false;
  }

  return true;
}

bool BombOrder::execute() {
  if (validate()) {
    // Halve the number of armies in target territory
    *(target->armiesNum) /= 2;

    // remove the card
    int i = 0;
    for (Card *card : *issuer->getHand()->cards) {
      if (card->type == CardType::Bomb) {
        issuer->getHand()->playCard(i, *issuer->getDeck());
        break;
      }
      i++;
    }

    setIsExecuted(true);
    setEffect("Bombed " + *(target->getName()) + "successfully.");
    return true;
  }
  return false;
}

//-------------BLOCKADE ORDER IMPLEMENTATION--------------//
// Default Constructor
BlockadeOrder::BlockadeOrder() : Order("Blockade", nullptr), target(nullptr) {}

// Parameterized Constructor
BlockadeOrder::BlockadeOrder(Player *issuer, Territory *target)
    : Order("Blockade", issuer), target(target) {}

// Getters
Territory *BlockadeOrder::getTargetTerritory() const { return target; }

// Setters
void BlockadeOrder::setTargetTerritory(Territory *target) {
  this->target = target;
}

// Methods
bool BlockadeOrder::validate() {
  // Check if target territory or issuer is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Blockade order validation failed. Target territory or issuer is "
              "null.");
    return false;
  }
  // Check if the player has a Blockade card
  bool hasCard = false;
  for (Card *card : *issuer->getHand()->cards) {
    if (card->type == CardType::Blockade) {
      hasCard = true;
      break;
    }
  }
  if (!hasCard) {
    setIsExecuted(false);
    setEffect("Blockade order validation failed. Issuer does not have a "
              "Blockade card.");
    return false;
  }

  // Target must belong to the player that issued the order
  const vector<Territory *> playerTerritories = issuer->getTerritories();
  bool targetOwned = false;
  for (Territory *territory : playerTerritories) {
    if (territory == target) {
      targetOwned = true;
      break;
    }
  }
  if (!targetOwned) {
    setIsExecuted(false);
    setEffect("Blockade order validation failed. Target territory does not "
              "belong to the issuer.");
    return false;
  }
  return true;
}

bool BlockadeOrder::execute() {
  if (validate()) {
    // Double the number of armies (spec says double, not triple)
    target->setArmiesNum(target->getArmiesNum() * 2);
    // Transfer ownership to Neutral (playerId = -1 represents Neutral)
    *target->playerId = -1;
    // Remove territory from issuer's list
    issuer->removeTerritory(target);

    // remove the card
    int i = 0;
    for (Card *card : *issuer->getHand()->cards) {
      if (card->type == CardType::Blockade) {
        issuer->getHand()->playCard(i, *issuer->getDeck());
        break;
      }
      i++;
    }

    setIsExecuted(true);
    setEffect("Blockaded territory " + *(target->getName()) +
              ", doubling its armies and transferring to Neutral player.");
    return true;
  }
  return false;
}

//-------------AIRLIFT ORDER IMPLEMENTATION--------------//
// Default Constructor
AirliftOrder::AirliftOrder()
    : Order("Airlift", nullptr), numArmies(0), source(nullptr),
      target(nullptr) {}

// Parameterized Constructor
AirliftOrder::AirliftOrder(Player *issuer, int numArmies, Territory *source,
                           Territory *target)
    : Order("Airlift", issuer), numArmies(numArmies), source(source),
      target(target) {}

// Getters
int AirliftOrder::getNumArmies() const { return numArmies; }

Territory *AirliftOrder::getSourceTerritory() { return source; }

Territory *AirliftOrder::getTargetTerritory() { return target; }

// Setters
void AirliftOrder::setNumArmies(const int numArmies) {
  this->numArmies = numArmies;
}
void AirliftOrder::setSourceTerritory(Territory *source) {
  this->source = source;
}
void AirliftOrder::setTargetTerritory(Territory *target) {
  this->target = target;
}
// Methods
bool AirliftOrder::validate() {
  // Check if source/target/issuer is null
  if (source == nullptr || target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Source/target territory or "
              "issuer is null.");
    return false;
  }

  if (*(source->playerId) != issuer->getId()) {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Source territory does not "
              "belong to the issuer.");
    return false;
  }
  if (*(target->playerId) != issuer->getId()) {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Target territory does not "
              "belong to the issuer.");
    return false;
  }

  bool hasCard = false;
  for (Card *card : *issuer->getHand()->cards) {
    if (card->type == CardType::Airlift) {
      hasCard = true;
      break;
    }
  }
  if (!hasCard) {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Issuer does not have a Airlift "
              "card.");
    return false;
  }
  
  return true;
}

bool AirliftOrder::execute() {
  if (validate()) {
    // Add armies to the target territory and remove them from source territory
    *(source->armiesNum) -= numArmies;
    *(target->armiesNum) += numArmies;

    // remove the card
    int i = 0;
    for (Card *card : *issuer->getHand()->cards) {
      if (card->type == CardType::Airlift) {
        issuer->getHand()->playCard(i, *issuer->getDeck());
        break;
      }
      i++;
    }

    setIsExecuted(true);
    setEffect("Airlifted " + to_string(numArmies) + " armies from " +
              *(source->getName()) + " to " + *(target->getName()));
    return true;
  }
  return false;
}

//-------------NEGOTIATE ORDER IMPLEMENTATION--------------//
// Default Constructor
NegotiateOrder::NegotiateOrder()
    : Order("Negotiate", nullptr), target(nullptr) {}

// Parameterized Constructor
NegotiateOrder::NegotiateOrder(Player *issuer, Player *targetPlayer)
    : Order("Negotiate", issuer), target(targetPlayer) {}

// Getters
Player *NegotiateOrder::getTargetPlayer() const { return target; }

// Setters
void NegotiateOrder::setTargetPlayer(Player *targetPlayer) {
  this->target = targetPlayer;
}

// Methods
bool NegotiateOrder::validate() {
  // Check if target player or issuer is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect(
        "Negotiate order validation failed. Target player or issuer is null.");
    return false;
  }
  // A player cannot negotiate with themselves
  if (target == issuer) {
    setIsExecuted(false);
    setEffect(
        "Negotiate order validation failed. Cannot negotiate with yourself.");
    return false;
  }
  // check if the issuer has a diplomacy card
  bool hasCard = false;
  for (Card *card : *issuer->getHand()->cards) {
    if (card->type == CardType::Diplomacy) {
      hasCard = true;
      break;
    }
  }
  if (!hasCard) {
    setIsExecuted(false);
    setEffect("Negotiate order validation failed. Issuer does not have a "
              "Negotiate card.");
    return false;
  }
  // Check if the issuer has already negotiated with the target player
  for (Player *negotiatedPlayer : issuer->getNegotiatedPlayers()) {
    if (negotiatedPlayer == target) {
      setIsExecuted(false);
      setEffect("Negotiate order validation failed. Issuer has already "
                "negotiated with this player.");
      return false;
    }
  }
  return true;
}

bool NegotiateOrder::execute() {
  if (validate()) {
    // Establish bidirectional peace treaty for the remainder of this turn
    issuer->addNegotiatedPlayer(target);
    target->addNegotiatedPlayer(issuer);

    // remove the card
    int i = 0;
    for (Card *card : *issuer->getHand()->cards) {
      if (card->type == CardType::Diplomacy) {
        issuer->getHand()->playCard(i, *issuer->getDeck());
        break;
      }
      i++;
    }

    setIsExecuted(true);
    setEffect("Negotiated peace between " + issuer->getName() + " and " +
              target->getName() + ". Neither can attack the other this turn.");
    return true;
  }
  return false;
}

//-------------ORDERLIST IMPLEMENTATION--------------//
// Default Constructor
OrderList::OrderList() { orders = new vector<Order *>(); }

// Parameterized Constructor
OrderList::OrderList(vector<Order *> *orders) : orders(orders) {}

// Copy constructor
OrderList::OrderList(const OrderList &other) {
  this->orders = new vector<Order *>();
  for (Order *order : *(other.orders)) {
    this->orders->push_back(new Order(*order));
  }
}

// Assignment operator
OrderList &OrderList::operator=(const OrderList &other) {
  if (this != &other) {
    for (auto order : *orders)
      delete order;
    delete orders;

    this->orders = new vector<Order *>();
    for (Order *order : *(other.orders)) {
      this->orders->push_back(new Order(*order));
    }
  }
  return *this;
}

// Destuctor
OrderList::~OrderList() {
  for (auto order : *orders)
    delete order;
  delete orders;
}

// Getters
vector<Order *> *OrderList::getOrders() const { return orders; }

// Methods

void OrderList::addOrder(Order *order) { orders->push_back(order); }

void OrderList::move(int currentIndex, int newIndex) {
  if (currentIndex < newIndex) {
    std::rotate(orders->begin() + currentIndex,
                orders->begin() + currentIndex + 1,
                orders->begin() + newIndex + 1);
  } else {
    std::rotate(orders->begin() + newIndex, orders->begin() + currentIndex,
                orders->begin() + currentIndex + 1);
  }
}
void OrderList::remove(int index) {
  if (index >= 0 && index < orders->size()) {
    delete (*orders)[index];
    orders->erase(orders->begin() + index);
  }
}

std::ostream &operator<<(std::ostream &os, const OrderList &orderList) {
  os << "Order List: " << std::endl;
  if (orderList.orders->empty()) {
    os << "No orders in the list." << std::endl;
    return os;
  } else {
    for (size_t i = 0; i < orderList.orders->size(); ++i) {
      os << i + 1 << ". " << *(orderList.orders->at(i)) << std::endl;
    }
    return os;
  }
}
