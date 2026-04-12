#ifndef ORDERS_H
#define ORDERS_H

#include "../map/Map.h"
#include "../player/Player.h"
#include "../game-engine/GameEngine.h"
#include "../Observer/LoggingObserver.h"
#include <iostream>
#include <string>
#include <vector>

// Order class implementation
class Order: public ILoggable, public Subject {

protected:
  std::string orderType;
  Player *issuer;
  bool isExecuted;
  std::string orderEffect;

public:
  Order();
  Order(std::string type, Player *player);
  Order(const Order &other) = default;
  Order &operator=(const Order &other) = default;
  virtual ~Order() = default;
  // Getters
  std::string getOrderType() const;
  Player *getPlayer() const;
  bool getIsExecuted() const;
  std::string getOrderEffect() const;
  // Setters
  void setOrderType(const std::string orderType);
  void setPlayer(Player *player);
  void setIsExecuted(const bool isExecuted);
  void setEffect(const std::string orderEffect);
  // Methods
  virtual bool validate();
  virtual bool execute();

  static GameEngine* gameEngine;
  std::string stringToLog() const override;
};

class DeployOrder : public Order {
private:
  int numArmies;
  Territory *targetTerritory;

public:
  DeployOrder();
  DeployOrder(Player *issuer, int numArmies, Territory *targetTerritory);
  DeployOrder(const DeployOrder &other) = default;
  DeployOrder &operator=(const DeployOrder &other) = default;
  ~DeployOrder() override = default;
  // Getters
  int getNumArmies() const;
  Territory *getTargetTerritory() const;
  // Setters
  void setArmies(const int numArmies);
  void setTerritory(Territory *targetTerritory);
  // Methods
  bool validate() override;
  bool execute() override;
};

class AdvanceOrder : public Order {
private:
  int numArmies;
  Territory *source;
  Territory *target;
  bool cheat;
public:
  AdvanceOrder();
  AdvanceOrder(Player* issuer, int numArmies, Territory* source,
      Territory* target, bool cheat = false);
  AdvanceOrder(const AdvanceOrder &other) = default;
  AdvanceOrder &operator=(const AdvanceOrder &other) = default;
  ~AdvanceOrder() override = default;
  // Getters
  int getNumArmies() const;
  Territory *getSourceTerritory();
  Territory *getTargetTerritory();
  // Setters
  void setNumArmies(const int numArmies);
  void setSourceTerritory(Territory *source);
  void setTargetTerritory(Territory *target);
  // Methods
  bool validate() override;
  bool execute() override;
};

class BombOrder : public Order {
private:
  Territory *target;

public:
  BombOrder();
  BombOrder(Player *issuer, Territory *target);
  BombOrder(const BombOrder &other) = default;
  BombOrder &operator=(const BombOrder &other) = default;
  ~BombOrder() override = default;
  // Getters
  Territory *getTargetTerritory() const;
  // Setters
  void setTargetTerritory(Territory *target);
  // Methods
  bool validate() override;
  bool execute() override;
};

class BlockadeOrder : public Order {
private:
  Territory *target;

public:
  BlockadeOrder();
  BlockadeOrder(Player *issuer, Territory *target);
  BlockadeOrder(const BlockadeOrder &other) = default;
  BlockadeOrder &operator=(const BlockadeOrder &other) = default;
  ~BlockadeOrder() override = default;
  // Getters
  Territory *getTargetTerritory() const;
  // Setters
  void setTargetTerritory(Territory *target);
  // Methods
  bool validate() override;
  bool execute() override;
};

class AirliftOrder : public Order {
private:
  int numArmies;
  Territory *source;
  Territory *target;

public:
  AirliftOrder();
  AirliftOrder(Player *issuer, int numArmies, Territory *source,
               Territory *target);
  AirliftOrder(const AirliftOrder &other) = default;
  AirliftOrder &operator=(const AirliftOrder &other) = default;
  ~AirliftOrder() override = default;
  // Getters
  int getNumArmies() const;
  Territory *getSourceTerritory();
  Territory *getTargetTerritory();
  // Setters
  void setNumArmies(const int numArmies);
  void setSourceTerritory(Territory *source);
  void setTargetTerritory(Territory *target);
  // Methods
  bool validate() override;
  bool execute() override;
};

class NegotiateOrder : public Order {
private:
  Player *target;

public:
  NegotiateOrder();
  NegotiateOrder(Player *issuer, Player *target);
  NegotiateOrder(const NegotiateOrder &other) = default;
  NegotiateOrder &operator=(const NegotiateOrder &other) = default;
  ~NegotiateOrder() override = default;
  // Getters
  Player *getTargetPlayer() const;
  // Setters
  void setTargetPlayer(Player *target);
  // Methods
  bool validate() override;
  bool execute() override;
};

class OrderList: public ILoggable, public Subject {
private:
  vector<Order *> *orders;

public:
  OrderList();
  OrderList(vector<Order *> *orders);
  OrderList(const OrderList &other);
  OrderList &operator=(const OrderList &other);
  ~OrderList();
  // Getters
  vector<Order *> *getOrders() const;
  // Setters
  void setOrders(const vector<Order *> *orders);
  // Methods
  void addOrder(Order *order);
  void move(int currentIndex, int newIndex);
  void remove(int index);

  std::string stringToLog() const override;

  friend ostream &operator<<(ostream &os, const OrderList &ol);
};

#endif