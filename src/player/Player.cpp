#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "../game-engine/GameEngine.h"
#include "Player.h"

using std::cout, std::stringstream, std::string;

int *Player::nextId = new int(0);
MapLoader* Player::mapLoader = nullptr;
GameEngine* Player::gameEngine = nullptr; 

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
     << ", Territories owned: " << p.territories->size() 
     << ", Reinforcement pool:" <<p.reinforcementPool
     << ", Hand: " << *p.getHand();
  return os;
}

int Player::getId() const { return *id; }

std::string Player::getName() const { return *name; }

OrderList *Player::getOrders() const { return orders; }

const std::vector<Territory *> &Player::getTerritories() const {
  return *territories;
}

Hand *Player::getHand() const { return hand; }

Deck* Player::getDeck() { return deck; }

int Player::getReinforcementPool() const { return reinforcementPool; }

bool Player::getConqueredThisTurn() const { return conqueredThisTurn; }

const std::vector<Player *> &Player::getNegotiatedPlayers() const {
  return negotiatedPlayers;
}

void Player::setId(int id) {
  (*this->id) = id;
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

// return territories of this player
std::vector<Territory *> Player::toDefend() const { return *territories; }

// return a list of neighbor territories of oposing players
std::vector<Territory *> Player::toAttack() const {
  int territoriesN = mapLoader->getTerritoriesNum();
  vector <bool> arr(territoriesN, false);
  for (auto territory : getTerritories()) {
    int territoryId_a = *territory->id;
    for (auto territoryId_b : *mapLoader->getTerritoryNeighborsIds(territoryId_a)) {
      if (mapLoader->getTerritoryPlayerId(*territoryId_b) != *id) {
        arr[*territoryId_b] = true;
      }
    }
  }

  vector <Territory*> list;

  for (int i = 0; i < territoriesN; i++) {
    if (arr[i]) {
      list.push_back((*mapLoader->map->territories)[i]);
    }
  }

  return list;
}

void Player::issueOrder(string input) {
  Order *order = nullptr;

  string commandType;
  stringstream ss(input);

  auto defendTerritories = toDefend();
  auto attackTerritories = toAttack();
  
  
  string orderType;
  ss >> orderType;

  if (orderType == "deploy") {
    int armNum, territoryId;
    // check the arguments
    if (ss >> armNum >> territoryId) {

      // check if territoryId is ok (is in defend list)
      Territory* territory = nullptr;
      for (auto x : defendTerritories) {
        if (*x->id == territoryId) {
          territory = x;
          break;
        }
      }
      if (territory == nullptr) {
        cout << "Error: Territory id is not from the defend list\n";
      } else if (armNum > reinforcementPool || armNum <= 0) {
        cout << "Error: Armies number should be between 1 and the reinforcement pool\n";
      } else {
        reinforcementPool -= armNum;
        order = new DeployOrder(this, armNum, territory);
      }
    }
  } else {
    if (reinforcementPool == 0) {
      if (orderType == "advance") {
        int armNum, sourceTerritoryId, targetTerritoryId;
        if (ss >> armNum >> sourceTerritoryId >> targetTerritoryId) {
          Territory *sourceTerritory = nullptr, *targetTerritory = nullptr;

          // sourceTerritory should be from defend list
          // targetTerritory should be from defend or attack list

          for (auto x : defendTerritories) {
            if (*x->id == sourceTerritoryId) {
              sourceTerritory = x;
            }
            if (*x->id == targetTerritoryId) {
              targetTerritory = x;
            }
          }

          for (auto x : attackTerritories) {
            if (*x->id == targetTerritoryId) {
              targetTerritory = x;
            }
          }

          if (targetTerritory == nullptr || sourceTerritory == nullptr) {
            cout << "Error: Source and target territories should be from defend and defend/attack lists respectively\n";
          } else {
            order = new AdvanceOrder(this, armNum, sourceTerritory, targetTerritory);
          }
        }
      }
      else if (orderType == "airlift") {
        int armNum, sourceTerritoryId, targetTerritoryId; 
        if (ss >> armNum >> sourceTerritoryId >> targetTerritoryId) {
          Territory *sourceTerritory = nullptr, *targetTerritory = nullptr;

          for (auto x : *mapLoader->map->territories) {
            if (*x->id == sourceTerritoryId) {
              sourceTerritory = x;
            }
            if (*x->id == targetTerritoryId) {
              targetTerritory = x;
            }
          }

          order = new AirliftOrder(this, armNum, sourceTerritory, targetTerritory);
        }
      }
      else if (orderType == "bomb") {
        int targetTerritoryId;
        if (ss >> targetTerritoryId) {
          Territory *targetTerritory = nullptr;

          for (auto x : *mapLoader->map->territories) {
            if (*x->id == targetTerritoryId) {
              targetTerritory = x;
            }
          }

          order = new BombOrder(this, targetTerritory);
        }
      }
      else if (orderType == "blockade") {
        int targetTerritoryId;
        if (ss >> targetTerritoryId) {
          Territory *targetTerritory = nullptr;

          for (auto x : *mapLoader->map->territories) {
            if (*x->id == targetTerritoryId) {
              targetTerritory = x;
            }
          }

          order = new BlockadeOrder(this, targetTerritory);
        }
      }
      else if (orderType == "negotiate") {
        int opponentPlayerId;
        if (ss >> opponentPlayerId) {
          Player* opponentPlayer = nullptr;

          // find opponentPlayer between active players
          for (int i = 0; i < gameEngine->getPlayerCount(); i++) {
            if (gameEngine->getPlayers()[i]->getTerritories().size() > 0 && gameEngine->getPlayers()[i]->getId() == opponentPlayerId) {
              opponentPlayer = gameEngine->getPlayers()[i];
            }
          }

          order = new NegotiateOrder(this, opponentPlayer);
        }
      }
    } else {
      cout << "Error: You have to first deploy your reinforcement pool\n";
    }
  }
  

  if (order == nullptr) {
    cout << "Error: There is a problem with the order issued\n\n";
  } else {
    orders->addOrder(order);
    cout << "Success: Order issued successfully\n\n";
  }
}