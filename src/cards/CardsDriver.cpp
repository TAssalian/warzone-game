#include "Cards.h"
#include <iostream>

using std::cout, std::endl;

int run_cards_driver() {
  cout << "=== CARDS DRIVER ===" << endl << endl;

  // 1. Create a deck with all 5 card types (multiple of each)
  int deckSize = 20;
  std::vector<Card *> *cards = new std::vector<Card *>();
  for (int i = 0; i < deckSize; i++) {
    cards->push_back(new Card(static_cast<CardType>(i % 5)));
  }

  Deck *deck = new Deck(*cards);
  cout << "--- Created deck ---" << endl;
  cout << *deck << endl;
  cout << "Deck size: " << deck->size() << endl << endl;

  // 2. Draw a hand of 6 cards
  Hand *hand = new Hand(deck);
  cout << "--- Drew hand from deck ---" << endl;
  cout << *hand << endl;
  cout << "Deck size after draw: " << deck->size() << endl << endl;

  // 3. Play each card from the hand, returning it to the deck
  cout << "--- Playing all cards from hand ---" << endl;
  int handSize = hand->size();
  for (int i = 0; i < handSize; i++) {
    cout << "Playing card at index 0:" << endl;
    hand->playCard(0, *deck);
    cout << "  Hand: " << *hand << endl;
    cout << "  Deck size: " << deck->size() << endl << endl;
  }

  // Verify hand is empty and deck is back to original size
  cout << "Hand size after playing all: " << hand->size() << endl;
  cout << "Deck size after returning all: " << deck->size() << endl;
  cout << endl;

  // 4. Draw a new hand and test copy constructor
  Hand *hand2 = new Hand(deck);
  cout << "--- Testing copy constructor ---" << endl;
  cout << "Original hand: " << *hand2 << endl;

  Hand hand3(*hand2);
  cout << "Copied hand:   " << hand3 << endl << endl;

  // 5. Test assignment operator
  cout << "--- Testing assignment operator ---" << endl;
  Hand *hand4 = new Hand(deck);
  cout << "hand4 before assignment: " << *hand4 << endl;
  *hand4 = *hand2;
  cout << "hand4 after  assignment: " << *hand4 << endl;
  cout << "hand2 (source):          " << *hand2 << endl << endl;

  // 6. Test Card copy constructor and assignment
  cout << "--- Testing Card copy/assignment ---" << endl;
  Card original(Bomb);
  Card copy(original);
  cout << "Original: " << original << endl;
  cout << "Copy:     " << copy << endl;

  Card assigned(Airlift);
  assigned = original;
  cout << "Assigned: " << assigned << endl << endl;

  // 7. Test Deck copy constructor
  cout << "--- Testing Deck copy constructor ---" << endl;
  cout << "Original deck: " << *deck << endl;
  Deck deckCopy(*deck);
  cout << "Copied deck:   " << deckCopy << endl << endl;

  // 8. Test operator<< for stream insertion
  cout << "--- Testing operator<< ---" << endl;
  cout << "Card:  " << Card(Diplomacy) << endl;
  cout << "Deck:  " << *deck << endl;
  cout << "Hand:  " << *hand2 << endl << endl;

  // Cleanup
  delete hand;
  delete hand2;
  delete hand4;
  delete deck;

  cout << "=== CARDS DRIVER COMPLETE ===" << endl;
  return 0;
}

int main() {
  run_cards_driver();
  return 0;
}