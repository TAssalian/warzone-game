#include "../cards/Cards.h"
#include "../map/Map.h"
#include "../player/Player.h"
#include "Orders.h"
#include <iostream>
#include <vector>

using namespace std;

int run_orders_driver() {
  std::vector<Card *> *cards = new std::vector<Card *>();
  for (int i = 0; i < 20; i++) {
    cards->push_back(new Card(static_cast<CardType>(i % 5)));
  }
  Deck *deck = new Deck(*cards);

  // Create players
  Player *player1 = new Player("Alice", deck);
  Player *player2 = new Player("Bob", deck);
  Player *player3 = new Player("Charlie", deck);

  // Clear hands filled with random 6 default cards
  player1->getHand()->cards->clear();
  player2->getHand()->cards->clear();
  player3->getHand()->cards->clear();

  // Give Alice cards
  player1->getHand()->cards->push_back(new Card(CardType::Bomb));
  player1->getHand()->cards->push_back(new Card(CardType::Blockade));
  player1->getHand()->cards->push_back(new Card(CardType::Airlift));
  player1->getHand()->cards->push_back(new Card(CardType::Diplomacy));

  // Give Bob cards
  player2->getHand()->cards->push_back(new Card(CardType::Airlift));
  player2->getHand()->cards->push_back(new Card(CardType::Diplomacy));

  // Give Charlie cards
  player3->getHand()->cards->push_back(new Card(CardType::Bomb));

  // Create territories
  Territory *t1 = new Territory(1, "t1", 1);
  Territory *t2 = new Territory(2, "t2", 1);
  Territory *t3 = new Territory(3, "t3", 1);
  Territory *t4 = new Territory(4, "t4", 1);

  // Setup adjacencies
  t1->getNeighborsIds()->push_back(new int(2));
  t2->getNeighborsIds()->push_back(new int(1));
  t2->getNeighborsIds()->push_back(new int(3));
  t3->getNeighborsIds()->push_back(new int(2));
  t4->getNeighborsIds()->push_back(new int(3));
  t3->getNeighborsIds()->push_back(new int(4));

  // Assign ownership
  *t1->playerId = player1->getId();
  *t2->playerId = player2->getId();
  *t3->playerId = player3->getId();
  *t4->playerId = player2->getId();

  player1->addTerritory(t1);
  player2->addTerritory(t2);
  player3->addTerritory(t3);
  player2->addTerritory(t4);

  t1->setArmiesNum(10);
  t2->setArmiesNum(3);
  t3->setArmiesNum(5);
  t4->setArmiesNum(2);

  // Set reinforcement pool
  player1->setReinforcementPool(5);

  cout << "Players created: " << player1->getName()
       << " (ID: " << player1->getId() << ")"
       << ", " << player2->getName() << " (ID: " << player2->getId() << ")"
       << ", " << player3->getName() << " (ID: " << player3->getId() << ")"
       << endl;

  cout << "\nDemonstrating Deploy Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting valid deploy (Alice to t1):" << endl;
  DeployOrder *validDeploy = new DeployOrder(player1, 5, t1);
  validDeploy->execute();
  cout << validDeploy->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid deploy (Alice to t2 owned by Bob):" << endl;
  DeployOrder *invalidDeploy = new DeployOrder(player1, 5, t2);
  invalidDeploy->execute();
  cout << invalidDeploy->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid deploy (Alice tries to deploy 6 armies, but only "
          "has "
       << player1->getReinforcementPool() << " left):" << endl;
  DeployOrder *invalidArmyDeploy = new DeployOrder(player1, 6, t1);
  invalidArmyDeploy->execute();
  cout << invalidArmyDeploy->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "\nDemonstrating Advance Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting invalid advance (Alice advances from t2 [Bob's] "
          "to t1):"
       << endl;
  AdvanceOrder *invalidSourceAdvance = new AdvanceOrder(player1, 5, t2, t1);
  invalidSourceAdvance->execute();
  cout << invalidSourceAdvance->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid advance (Alice advances from t1 to t4 "
          "[Not adjacent]):"
       << endl;
  AdvanceOrder *invalidAdjAdvance = new AdvanceOrder(player1, 5, t1, t4);
  invalidAdjAdvance->execute();
  cout << invalidAdjAdvance->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Alice attacks Bob (t1 -> t2) with 10 armies." << endl;
  cout << "Pre-battle: Alice owns t2? "
       << (*t2->playerId == player1->getId() ? "Yes" : "No") << endl;
  cout << "Alice hand size: " << player1->getHand()->size() << endl;

  AdvanceOrder *validAttack = new AdvanceOrder(player1, 10, t1, t2);
  validAttack->execute();
  cout << validAttack->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Post-battle: Alice owns t2? "
       << (*t2->playerId == player1->getId() ? "Yes" : "No") << endl;
  cout << "Did Alice conquer a territory this turn? "
       << (player1->getConqueredThisTurn() ? "Yes" : "No") << endl;
  if (player1->getConqueredThisTurn()) {
    player1->getHand()->cards->push_back(new Card(deck->draw()));
    cout << "Alice new hand size: " << player1->getHand()->size() << endl;
  }
  cout << "\n\n" << endl;

  cout << "\nDemonstrating Airlift Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting invalid airlift (Alice [has card] airlifts from t4 "
          "[Bob's] to t1):"
       << endl;
  AirliftOrder *invalidAirlift =
      new AirliftOrder(player1, 5, t4, t1); // source, target
  invalidAirlift->execute();
  cout << invalidAirlift->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting valid airlift (Alice` [who has an Airlift card] airlifts "
          "from t2 to t1 [Her own]):"
       << endl;
  AirliftOrder *validAirlift =
      new AirliftOrder(player1, 2, t2, t1); // source, target
  validAirlift->execute();
  cout << validAirlift->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid airlift (Charlie [who has NO Airlift card] "
          "airlifts from t3 to t3):"
       << endl;
  AirliftOrder *invalidCardAirlift = new AirliftOrder(player3, 2, t3, t3);
  invalidCardAirlift->execute();
  cout << invalidCardAirlift->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "\nDemonstrating Bomb Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting invalid bomb (Alice bombs t1 [Her own]):" << endl;
  BombOrder *invalidOwnBomb = new BombOrder(player1, t1);
  invalidOwnBomb->execute();
  cout << invalidOwnBomb->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid bomb (Alice bombs t4 [Bob's, not "
          "adjacent to Alice]):"
       << endl;
  BombOrder *invalidAdjBomb = new BombOrder(player1, t4);
  invalidAdjBomb->execute();
  cout << invalidAdjBomb->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting valid bomb (Alice bombs t3 [Charlie's, Adjacent, "
          "Valid Card]):"
       << endl;
  BombOrder *validBomb = new BombOrder(player1, t3);
  validBomb->execute();
  cout << validBomb->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid bomb (Bob [who has NO Bomb card] bombs t1):"
       << endl;
  BombOrder *invalidCardBomb = new BombOrder(player2, t1);
  invalidCardBomb->execute();
  cout << invalidCardBomb->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "\nDemonstrating Negotiate Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting invalid negotiate (Alice negotiates with herself):"
       << endl;
  NegotiateOrder *invalidSelfNegotiate = new NegotiateOrder(player1, player1);
  invalidSelfNegotiate->execute();
  cout << invalidSelfNegotiate->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Attempting invalid negotiate (Charlie [who has NO Diplomacy card] "
          "negotiates with Alice):"
       << endl;
  NegotiateOrder *invalidCardNegotiate = new NegotiateOrder(player3, player1);
  invalidCardNegotiate->execute();
  cout << invalidCardNegotiate->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Bob [has Diplomacy card] negotiates with Charlie." << endl;
  NegotiateOrder *validNegotiate = new NegotiateOrder(player2, player3);
  validNegotiate->execute();
  cout << validNegotiate->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Bob tries to negotiate with Charlie a second time:" << endl;
  NegotiateOrder *dupNegotiate = new NegotiateOrder(player2, player3);
  dupNegotiate->execute();
  cout << dupNegotiate->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Bob tries to attack Charlie (t4 -> t3) after negotiating:" << endl;
  // Let's pretend Alberta is adjacent to Greenland for this specific test
  t4->getNeighborsIds()->push_back(new int(3));
  AdvanceOrder *blockedAttack = new AdvanceOrder(player2, 2, t4, t3);
  blockedAttack->execute();
  cout << blockedAttack->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "Charlie tries to bomb Bob (t3 -> t4) after negotiating:" << endl;
  BombOrder *blockedBomb = new BombOrder(player3, t4);
  blockedBomb->execute();
  cout << blockedBomb->getOrderEffect() << endl;
  cout << "\n\n" << endl;

  cout << "\nDemonstrating Blockade Order" << endl;
  cout << "-----------------------------" << endl;
  cout << "Attempting valid blockade (Alice blockades t1):" << endl;
  cout << "Pre-Blockade (Owner ID: " << *t1->playerId
       << ", Armies: " << *t1->getArmiesNum() << ")" << endl;
  BlockadeOrder *validBlockade = new BlockadeOrder(player1, t1);
  validBlockade->execute();
  cout << validBlockade->getOrderEffect() << endl;
  cout << "Post-Blockade (Owner ID: " << *t1->playerId
       << ", Armies: " << *t1->getArmiesNum() << ")" << endl;
  cout << "\n\n" << endl;

  return 0;
}

int main() {
  run_orders_driver();
  return 0;
}