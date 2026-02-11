#include <algorithm>

#include "Player.h"
#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"


int* Player::nextId = new int(0);


Player::Player(std::string name, Deck* deck)
    : id(new int((*nextId)++)),
      name(new std::string(name)),
      territories(new std::vector<Territory*>()),
	  deck(deck),
	  hand(new Hand(deck)),
      orders(new OrderList()){}

Player::Player(const Player& other)
    : id(new int(*other.id)),
      name(new std::string(*other.name)),
      territories(new std::vector<Territory*>(*other.territories)),
	  deck(other.deck),
      hand(new Hand(*other.hand)),
	  orders(new OrderList(*other.orders)) {}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete id;
        delete name;
        delete territories;
		delete hand;
		delete orders;

        id = new int(*other.id);
        name = new std::string(*other.name);
        territories = new std::vector<Territory*>(*other.territories);
		deck = other.deck;
        hand = new Hand(*other.hand);
		orders = new OrderList(*other.orders);
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

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "Player ID: " << *p.id
       << ", Name: " << *p.name
       << ", Territories owned: " << p.territories->size();
    return os;
}

std::string Player::getName() const {
    return *name;
}

OrderList* Player::getOrders() const {
    return orders;
}

const std::vector<Territory*>& Player::getTerritories() const {
    return *territories;
}

Hand* Player::getHand() const {
    return hand;
}

void Player::addTerritory(Territory* territory) {
    territories->push_back(territory);
}

void Player::removeTerritory(Territory* territory) {
    territories->erase(
        std::remove(territories->begin(), territories->end(), territory),
        territories->end()
    );
}

// Both return arbitrary lists
// Just return all territories
std::vector<Territory*> Player::toDefend() const {
    return *territories;
}

// Just return no territories
std::vector<Territory*> Player::toAttack() const {
    return std::vector<Territory*>();
}

void Player::issueOrder() {
	Order* order = new Order("PlaceholderOrder", this);
	orders->addOrder(order);
}