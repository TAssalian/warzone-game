#pragma once

#include <iostream>
#include <string>
#include <vector>

class Territory;
class Deck;
class Hand;
class OrderList;
class Player;
class MapLoader;
class GameEngine;

using std::string;

class Player {
private:
  int *id;
  static int *nextId;

  std::string *name;
  std::vector<Territory *> *territories;
  Deck *deck;
  Hand *hand;
  OrderList *orders;
  int reinforcementPool;
  bool conqueredThisTurn;
  std::vector<Player *> negotiatedPlayers;

public:
  Player(std::string name, Deck *deck);
  Player(const Player &other);
  Player &operator=(const Player &other);
  ~Player();


  friend std::ostream &operator<<(std::ostream &os, const Player &p);

  std::string getName() const;
  int getId() const;
  const std::vector<Territory *> &getTerritories() const;
  Hand *getHand() const;
  OrderList *getOrders() const;
  int getReinforcementPool() const;
  bool getConqueredThisTurn() const;
  const std::vector<Player *> &getNegotiatedPlayers() const;
  Deck* getDeck();

  void setId(int id);
  void setReinforcementPool(int pool);
  void setConqueredThisTurn(bool conquered);
  void addNegotiatedPlayer(Player *player);
  bool isNegotiatedWith(Player *player) const;
  void clearNegotiatedPlayers();

  void addTerritory(Territory *territory);
  void removeTerritory(Territory *territory);


  std::vector<Territory *> toDefend() const;
  std::vector<Territory *> toAttack() const;

  void issueOrder(string input);

  static MapLoader* mapLoader;
  static GameEngine* gameEngine;
};