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

  // Register players in the GameEngine so executions like AdvanceOrder can
  // access them
  gameEngine->getPlayers()[player1->getId()] = player1;
  gameEngine->getPlayers()[player2->getId()] = player2;
  gameEngine->getPlayers()[player3->getId()] = player3;

  // Assign strategies
  player1->setStrategy(new AggressivePlayerStrategy());
  player2->setStrategy(new BenevolentPlayerStrategy());
  player3->setStrategy(new NeutralPlayerStrategy());

  // Create territories
  Territory *t1 = new Territory(1, "t1", 1);
  Territory *t2 = new Territory(2, "t2", 1);
  Territory *t3 = new Territory(3, "t3", 1);
  Territory *t4 = new Territory(4, "t4", 1);

  // Provide a mock MapLoader
  MapLoader *mockLoader = new MapLoader("");
  mockLoader->map = new Map();
  mockLoader->map->territories = new std::vector<Territory *>();
  mockLoader->map->territories->push_back(t1);
  mockLoader->map->territories->push_back(t2);
  mockLoader->map->territories->push_back(t3);
  mockLoader->map->territories->push_back(t4);
  Player::mapLoader = mockLoader;

  // Setup adjacencies
  t1->getNeighborsIds()->push_back(new int(2));
  
  t2->getNeighborsIds()->push_back(new int(1));
  t2->getNeighborsIds()->push_back(new int(3));
  t2->getNeighborsIds()->push_back(new int(4));
  
  t3->getNeighborsIds()->push_back(new int(2));
  t3->getNeighborsIds()->push_back(new int(4));
  
  t4->getNeighborsIds()->push_back(new int(2));
  t4->getNeighborsIds()->push_back(new int(3));

  // Assign ownership
  *t1->playerId = player1->getId();
  *t2->playerId = player1->getId();
  *t3->playerId = player2->getId();
  *t4->playerId = player2->getId();

  player1->addTerritory(t1);
  player1->addTerritory(t2);
  player2->addTerritory(t3);
  player2->addTerritory(t4);

  t1->setArmiesNum(10);
  t2->setArmiesNum(2);
  t3->setArmiesNum(3);
  t4->setArmiesNum(2);

  // Set reinforcement pool
  player1->setReinforcementPool(5);

  std::cout << "Players created: " << player1->getName()
            << " (ID: " << player1->getId() << ")"
            << ", " << player2->getName() << " (ID: " << player2->getId() << ")"
            << ", " << player3->getName() << " (ID: " << player3->getId() << ")"
            << std::endl;

  std::cout << "\n=== TURN 1 ===" << std::endl;
  std::cout << "t1 has no enemy neighbors. Alice will deploy on t1 (strongest) and advance to t2 (friendly front line)." << std::endl;
  
  player1->getStrategy()->issueOrder(player1);
  
  std::cout << "\nPlayer 1 orders for Turn 1:" << std::endl;
  std::cout << *player1->getOrders() << std::endl;

  for (Order *order : *player1->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }
  player1->getOrders()->getOrders()->clear(); // Clear orders for next turn

  std::cout << "\n=== TURN 2 ===" << std::endl;
  std::cout << "t2 is now strongest and on the front lines against t3 and t4. Alice deploys to t2 and attacks EVERY enemy neighbor!" << std::endl;
  
  player1->setReinforcementPool(5);
  player1->getStrategy()->issueOrder(player1);
  
  std::cout << "\nPlayer 1 orders for Turn 2:" << std::endl;
  std::cout << *player1->getOrders() << std::endl;

  for (Order *order : *player1->getOrders()->getOrders()) {
    order->execute();
    std::cout << order->getOrderEffect() << std::endl;
  }

  return 0;
}