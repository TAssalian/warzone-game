#include "../cards/Cards.h"
#include "../game-engine/GameEngine.h"
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "Player.h"
#include "PlayerStrategies.h"
#include <iostream>
#include <vector>

int main() {
  GameEngine *gameEngine = new GameEngine();
  Order::gameEngine = gameEngine;

  std::vector<Card *> *cards = new std::vector<Card *>();
  for (int i = 0; i < 20; i++) {
    cards->push_back(new Card(static_cast<CardType>(i % 5)));
  }
  Deck *deck = new Deck(*cards);

  // Create players
  Player *player1 = new Player("Alice", deck);
  Player *player2 = new Player("Bob", deck);
  Player *player3 = new Player("Charlie", deck);
  Player *player4 = new Player("David", deck);

  // Register players in the GameEngine so executions like AdvanceOrder can
  // access them
  gameEngine->getPlayers()[player1->getId()] = player1;
  gameEngine->getPlayers()[player2->getId()] = player2;
  gameEngine->getPlayers()[player3->getId()] = player3;
  gameEngine->getPlayers()[player4->getId()] = player4;

  // Assign strategies
  player1->setStrategy(new AggressivePlayerStrategy());
  player2->setStrategy(new BenevolentPlayerStrategy());
  player3->setStrategy(new CheaterPlayerStrategy());
  player4->setStrategy(new NeutralPlayerStrategy());

  // Create territories
  Territory *t1 = new Territory(0, "t1", 1);
  Territory *t2 = new Territory(1, "t2", 1);
  Territory *t3 = new Territory(2, "t3", 1);
  Territory *t4 = new Territory(3, "t4", 1);
  Territory *t5 = new Territory(4, "t5", 1);
  Territory *t6 = new Territory(5, "t6", 1);
  Territory *t7 = new Territory(6, "t7", 1);
  Territory *t8 = new Territory(7, "t8", 1);

  // Provide a mock MapLoader
  MapLoader *mockLoader = new MapLoader("");
  mockLoader->map = new Map();
  mockLoader->map->territories = new std::vector<Territory *>();
  mockLoader->map->territories->push_back(t1);
  mockLoader->map->territories->push_back(t2);
  mockLoader->map->territories->push_back(t3);
  mockLoader->map->territories->push_back(t4);
  mockLoader->map->territories->push_back(t5);
  mockLoader->map->territories->push_back(t6);
  mockLoader->map->territories->push_back(t7);
  mockLoader->map->territories->push_back(t8);
  Player::mapLoader = mockLoader;

  // Setup adjacencies
  t1->getNeighborsIds()->push_back(new int(1));
  t1->getNeighborsIds()->push_back(new int(2));

  t2->getNeighborsIds()->push_back(new int(0));
  t2->getNeighborsIds()->push_back(new int(2));
  t2->getNeighborsIds()->push_back(new int(3));
  t2->getNeighborsIds()->push_back(new int(4));

  t3->getNeighborsIds()->push_back(new int(1));
  t3->getNeighborsIds()->push_back(new int(3));
  t3->getNeighborsIds()->push_back(new int(0));

  t4->getNeighborsIds()->push_back(new int(1));
  t4->getNeighborsIds()->push_back(new int(2));

  t5->getNeighborsIds()->push_back(new int(1));
  t5->getNeighborsIds()->push_back(new int(5));
  t5->getNeighborsIds()->push_back(new int(6));

  t6->getNeighborsIds()->push_back(new int(5));
  t6->getNeighborsIds()->push_back(new int(7));

  t7->getNeighborsIds()->push_back(new int(4));
  t7->getNeighborsIds()->push_back(new int(7));
  t7->getNeighborsIds()->push_back(new int(3));

  t8->getNeighborsIds()->push_back(new int(6));
  t8->getNeighborsIds()->push_back(new int(5));

  // Assign ownership
  *t1->playerId = player1->getId();
  *t2->playerId = player2->getId();
  *t3->playerId = player3->getId();
  *t4->playerId = player1->getId();
  *t5->playerId = player2->getId();
  *t6->playerId = player2->getId();
  *t7->playerId = player4->getId();
  *t8->playerId = player4->getId();

  player1->addTerritory(t1);
  player2->addTerritory(t2);
  player3->addTerritory(t3);
  player1->addTerritory(t4);
  player2->addTerritory(t5);
  player2->addTerritory(t6);
  player4->addTerritory(t7);
  player4->addTerritory(t8);

  t1->setArmiesNum(10);
  t2->setArmiesNum(5);
  t3->setArmiesNum(30);
  t4->setArmiesNum(15);
  t5->setArmiesNum(20);
  t6->setArmiesNum(2);
  t7->setArmiesNum(2);
  t8->setArmiesNum(10);

  // Set reinforcement pool
  player1->setReinforcementPool(5);
  player2->setReinforcementPool(5);
  player3->setReinforcementPool(5);
  player4->setReinforcementPool(5);

  std::cout << "Players created: " << player1->getName()
            << " (ID: " << player1->getId() << ")"
            << ", " << player2->getName() << " (ID: " << player2->getId() << ")"
            << ", " << player3->getName() << " (ID: " << player3->getId() << ")"
            << std::endl;

  std::cout << "Demonstrating Aggressive Player Strategy" << std::endl;
  player1->getStrategy()->issueOrder(player1);

  std::cout << "Order List of Aggressive Player: " << std::endl;
  std::cout << *player1->getOrders() << std::endl;

  std::cout << "Executing orders of Aggressive Player: " << std::endl;
  for (Order *order : *player1->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }
  std::cout << "Order List of Aggressive Player after execution: " << std::endl;
  std::cout << *player1->getOrders() << std::endl;

  std::cout << "Demonstrating Benevolent Player Strategy" << std::endl;
  player2->getStrategy()->issueOrder(player2);
  std::cout << "Order List of Benevolent Player: " << std::endl;
  std::cout << *player2->getOrders() << std::endl;
  std::cout << "Executing orders of Benevolent Player: " << std::endl;
  for (Order *order : *player2->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }
  std::cout << *player2->getOrders() << std::endl;

  std::cout << "Demonstrating Cheater Player Strategy" << std::endl;
  player3->getStrategy()->issueOrder(player3);
  std::cout << "Order List of Cheater Player: " << std::endl;
  std::cout << *player3->getOrders() << std::endl;
  std::cout << "Executing orders of Cheater Player: " << std::endl;
  for (Order *order : *player3->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }
  std::cout << *player3->getOrders() << std::endl;

  std::cout << "Demonstrating Neutral Player Strategy" << std::endl;
  player4->getStrategy()->issueOrder(player4);
  std::cout << "Order List of Neutral Player: " << std::endl;
  std::cout << *player4->getOrders() << std::endl;
  std::cout << "Executing orders of Neutral Player: " << std::endl;
  for (Order *order : *player4->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }
  std::cout << *player4->getOrders() << std::endl;

  AdvanceOrder *validAttack = new AdvanceOrder(player2, 10, t5, t7);
  validAttack->execute();
  std::cout << validAttack->getOrderEffect() << std::endl;

  std::cout << "Demonstrating Neutral Player Strategy after attack"
            << std::endl;
  player4->getStrategy()->issueOrder(player4);
  std::cout << "Order List of Neutral Player after attack: " << std::endl;
  std::cout << *player4->getOrders() << std::endl;
  std::cout << "Executing orders of Neutral Player after attack: " << std::endl;
  for (Order *order : *player4->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }

  return 0;
}