#include <algorithm>

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"

int *Player::nextId = new int(0);

Player::Player(std::string name, Deck *deck)
    : id(new int((*nextId)++)), name(new std::string(name)),
      territories(new std::vector<Territory *>()), deck(deck),
      hand(new Hand(deck)), orders(new OrderList()), reinforcementPool(0),
      conqueredThisTurn(false), negotiatedPlayers() {}

Player::Player(const Player &other)
    : id(new int(*other.id)), name(new std::string(*other.name)),
      territories(new std::vector<Territory *>(*other.territories)),
      deck(other.deck), hand(new Hand(*other.hand)),
      orders(new OrderList(*other.orders)),
      reinforcementPool(other.reinforcementPool),
      conqueredThisTurn(other.conqueredThisTurn),
      negotiatedPlayers(other.negotiatedPlayers) {}

Player &Player::operator=(const Player &other) {
  if (this != &other) {
    delete id;
    delete name;
    delete territories;
    delete hand;
    delete orders;

    id = new int(*other.id);
    name = new std::string(*other.name);
    territories = new std::vector<Territory *>(*other.territories);
    deck = other.deck;
    hand = new Hand(*other.hand);
    orders = new OrderList(*other.orders);
    reinforcementPool = other.reinforcementPool;
    conqueredThisTurn = other.conqueredThisTurn;
    negotiatedPlayers = other.negotiatedPlayers;
  }
  return *this;
}

Player::~Player() {
  delete id;
  delete name;
  delete territories;
  delete hand;
  delete orders;
}

std::ostream &operator<<(std::ostream &os, const Player &p) {
  os << "Player ID: " << *p.id << ", Name: " << *p.name
     << ", Territories owned: " << p.territories->size();
  return os;
}

int Player::getId() const { return *id; }

std::string Player::getName() const { return *name; }

OrderList *Player::getOrders() const { return orders; }

const std::vector<Territory *> &Player::getTerritories() const {
  return *territories;
}

Hand *Player::getHand() const { return hand; }

int Player::getReinforcementPool() const { return reinforcementPool; }

bool Player::getConqueredThisTurn() const { return conqueredThisTurn; }

const std::vector<Player *> &Player::getNegotiatedPlayers() const {
  return negotiatedPlayers;
}

void Player::setReinforcementPool(int pool) { reinforcementPool = pool; }

void Player::setConqueredThisTurn(bool conquered) {
  conqueredThisTurn = conquered;
}

void Player::addNegotiatedPlayer(Player *player) {
  negotiatedPlayers.push_back(player);
}

bool Player::isNegotiatedWith(Player *player) const {
  for (Player *p : negotiatedPlayers) {
    if (p == player)
      return true;
  }
  return false;
}

void Player::clearNegotiatedPlayers() { negotiatedPlayers.clear(); }

void Player::addTerritory(Territory *territory) {
  territories->push_back(territory);
}

void Player::removeTerritory(Territory *territory) {
  territories->erase(
      std::remove(territories->begin(), territories->end(), territory),
      territories->end());
}

// Both return arbitrary lists
// Just return all territories
std::vector<Territory *> Player::toDefend() const { return *territories; }

// Just return no territories
std::vector<Territory *> Player::toAttack() const {
  return std::vector<Territory *>();
}

void Player::issueOrder() {
  Order *order = new Order("PlaceholderOrder", this);
  orders->addOrder(order);
}