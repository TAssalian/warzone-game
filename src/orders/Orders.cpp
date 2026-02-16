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
bool DeployOrder::validate() { return true; }

bool DeployOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Deployed " + to_string(numArmies) + " armies to " +
              *(targetTerritory->getName()));
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Deploy order validation failed. Order has not been executed.");
    return false;
  }
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

bool AdvanceOrder::validate() { return true; }

bool AdvanceOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Advanced " + to_string(numArmies) +
              " armies between friendly territories from " +
              *(source->getName()) + " to " + *(target->getName()));
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Advance order validation failed. Order has not been executed.");
    return false;
  }
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
bool BombOrder::validate() { return true; }

bool BombOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Bombed " + *(target->getName()) + "successfully.");
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Bomb order validation failed. Order has not been executed.");
    return false;
  }
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
bool BlockadeOrder::validate() { return true; }

bool BlockadeOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Blockaded territory " + *(target->getName()) +
              ", tripling its armies and making it neutral.");
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Blockade order validation failed. Order has not been executed.");
    return false;
  }
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
bool AirliftOrder::validate() { return true; }

bool AirliftOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Airlifted " + to_string(numArmies) + " armies from " +
              *(source->getName()) + " to " + *(target->getName()));
    return true;
  } else {
    setIsExecuted(false);
    setEffect("Airlift order validation failed. Order has not been executed.");
    return false;
  }
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
void NegotiateOrder::setTargetPlayer(Player *target) { this->target = target; }

// Methods
bool NegotiateOrder::validate() { return true; }

bool NegotiateOrder::execute() {
  if (validate()) {
    setIsExecuted(true);
    setEffect("Negotiated peace between " + issuer->getName() + " and " +
              target->getName());
    return true;
  } else {
    setIsExecuted(false);
    setEffect(
        "Negotiate order validation failed. Order has not been executed.");
    return false;
  }
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
