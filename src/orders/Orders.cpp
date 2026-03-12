#include "Orders.h"
using namespace std;

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
    setEffect("Deploy order validation failed. Target territory or issuer is null.");
    return false;
  }

  // Check if the target territory belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory == targetTerritory) {
      // TODO: Check if player has enough reinforcement armies in pool
      return true;
    }
  }
  setIsExecuted(false);
  setEffect("Deploy order validation failed. Target territory does not belong to the issuer.");
  return false;
}

bool DeployOrder::execute() {
  if (validate()) {
    // Add armies to the target territory
    targetTerritory->setArmiesNum(*(targetTerritory->getArmiesNum()) + numArmies);
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
// Check if target territory is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Target territory or issuer is null.");
    return false;
  }

  // Check if the source territory belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory != source) {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Source territory does not belong to the issuer.");
      return false;
    }
  }

  //Check if the target territory is adjacent to the source territory
  vector<int*>* neighbourIds=source->getNeighborsIds();
  for(int* neighbourId:*neighbourIds){
    if(*neighbourId==*target->id){
      return true;
    }
  } 
  setIsExecuted(false);
  setEffect("Advance order validation failed. Target territory is not adjacent to source territory.");
  return false;
}

bool AdvanceOrder::execute() {
  if (validate()) {
    //If the target is owned by player move armies
    if(*(target->playerId) == *(source->playerId)){
      *(source->armiesNum) -= numArmies;
      *(target->armiesNum) += numArmies;
    }
    //Else attack the target
    else{
      int attackingArmies = numArmies;
      int defendingArmies = *(target->armiesNum);

      //Send the armies from source to target
			source->setArmiesNum(*(source->getArmiesNum() - numArmies));

      while (attackingArmies > 0 && defendingArmies > 0) {
					//Atacker has a 60% chance to kill a defender army
					bool attackerRoll = (rand() % 100) < 60;
					//Defender has a 70% chance to kill an attacker army
					bool defenderRoll = (rand() % 100) < 70;

					if (attackerRoll) {
						defendingArmies--;
					}
					if (defenderRoll) {
						attackingArmies--;
					}
			}

      //Defender wins
      if(defendingArmies>0){
        target->setArmiesNum(defendingArmies);
        setIsExecuted(true);
        setEffect("Attacked " + *(target->getName()) + " and lost.)");
      }
      //Attacker wins
      else{
        target->setArmiesNum(attackingArmies);
        target->id=source->playerId;
        setIsExecuted(true);
        setEffect("Attacked " + *(target->getName()) + " and conquered it.)");
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
  // Check if target territory is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Bomb order validation failed. Target territory or issuer is null.");
    return false;
  }

  //check if target belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory == target) {
      setIsExecuted(false);
      setEffect("Bomb order validation failed. Target territory belongs to the issuer.");
      return false; 
    }
  }

  //check if the target is adjacent to any of the players territories
  for(Territory *territory:playerTerritories){
    vector<int*>* neighbourIds=territory->getNeighborsIds();
    for(int* neighbourId:*neighbourIds){
      if(*neighbourId==*target->id){
        return true;  
      }
    }
  }
  setIsExecuted(false);
  setEffect("Bomb order validation failed. Target territory is not adjacent to any of the issuer territories.");
  return true; 
}

bool BombOrder::execute() {
  if (validate()) {
    //Halve the number of armies in target territory
    *(target->armiesNum) /= 2;
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
  // Check if target territory is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Blockade order validation failed. Target territory or issuer is null.");
    return false;
  }

  //check if target belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory != target) {
      setIsExecuted(false);
      setEffect("Blockade order validation failed. Target territory belongs to the enemy.");
      return false; 
    }
  }
  return true; 
}

bool BlockadeOrder::execute() {
  if (validate()) {
    //Triple the number of armies in target territory
    *(target->armiesNum) *= 3;
    //TO-DO: Set the owner of target territory to neutral
    setIsExecuted(true);
    setEffect("Blockaded territory " + *(target->getName()) +
              ", tripling its armies and making it neutral.");
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
AirliftOrder::AirliftOrder(Player *issuer, int numArmies, Territory *target,
                           Territory *source)
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
  // Check if target territory is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Target territory or issuer is null.");
    return false;
  }

  //check if source and target belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory != source) {
      setIsExecuted(false);
      setEffect("Airlift order validation failed. Source territory does not belong to the issuer.");
      return false; 
    }
  }
  for(Territory *territory : playerTerritories) {
    if (territory != target) {
      setIsExecuted(false);
      setEffect("Airlift order validation failed. Target territory does not belong to the issuer.");  
      return false; 
    }
  }
  return true;
}

bool AirliftOrder::execute() {
  if (validate()) {
    //Add armies to the target territory and remove them from source territory
    *(source->armiesNum) -= numArmies;
    *(target->armiesNum) += numArmies;
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
NegotiateOrder::NegotiateOrder(Player *issuer, Territory *targetPlayer)
    : Order("Negotiate", issuer), target(targetPlayer) {}

// Getters
Territory *NegotiateOrder::getTargetTerritory() const { return target; }

// Setters
void NegotiateOrder::setTargetTerritory(Territory *target) { this->target = target; }

// Methods
bool NegotiateOrder::validate() { 
  // Check if target territory is null
  if (target == nullptr || issuer == nullptr) {
    setIsExecuted(false);
    setEffect("Negotiate order validation failed. Target territory or issuer is null.");
    return false;
  }

  //check if source and target belongs to the player that issued the order
  const vector<Territory *> &playerTerritories = issuer->getTerritories();
  for (Territory *territory : playerTerritories) {
    if (territory == target) {
      setIsExecuted(false);
      setEffect("Negotiate order validation failed. Target territory belongs to the issuer.");
      return false; 
    }
  }
  return true;
}

bool NegotiateOrder::execute() {
  if (validate()) {
    //TO-DO: Implement the logic to prevent players from attacking each other until the end of the current turn
    setIsExecuted(true);
    setEffect("Negotiated peace between " + issuer->getName() + " and " + *(target->getName()));
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
