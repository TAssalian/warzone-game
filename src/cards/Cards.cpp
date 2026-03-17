#include "Cards.h"
#include <cassert>
#include <iostream>

std::string cardTypeName(CardType t) {
  switch (t) {
  case Bomb:
    return "Bomb";
  case Reinforcement:
    return "Reinforcement";
  case Blockade:
    return "Blockade";
  case Airlift:
    return "Airlift";
  case Diplomacy:
    return "Diplomacy";
  default:
    return "Unknown";
  }
}

// ------------- Card Implementation ---------------

Card::Card(CardType type) : type(type) {}

Card::Card(const Card &other) : type(other.type) {}

Card &Card::operator=(const Card &other) {
  if (this != &other) {
    type = other.type;
  }
  return *this;
}

Card::~Card() {}

void Card::play() {
  std::cout << "Playing card: " << cardTypeName(type) << std::endl;
  switch (type) {
  case Bomb:
    std::cout << "  -> Creates a Bomb order." << std::endl;
    break;
  case Reinforcement:
    std::cout << "  -> Creates a Deploy order." << std::endl;
    break;
  case Blockade:
    std::cout << "  -> Creates a Blockade order." << std::endl;
    break;
  case Airlift:
    std::cout << "  -> Creates an Airlift order." << std::endl;
    break;
  case Diplomacy:
    std::cout << "  -> Creates a Negotiate order." << std::endl;
    break;
  }
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
  os << "Card(" << cardTypeName(card.type) << ")";
  return os;
}

// ------------- Deck Implementation ---------------

Deck::Deck()
{
    // By default will generate a deck of 50 cards with all 5 card types (multiple of each)
    int deckSize = 50;
    cards = new std::vector<Card*>();
    for (int i = 0; i < deckSize; i++) {
        cards->push_back(new Card(static_cast<CardType>(i % 5)));
    }
}

Deck::Deck(std::vector<Card *> &cards) : cards(&cards) {}

Deck::Deck(const Deck &other) {
  cards = new std::vector<Card *>();
  for (auto card : *other.cards) {
    cards->push_back(new Card(*card));
  }
}

Deck &Deck::operator=(const Deck &other) {
  if (this != &other) {
    for (auto card : *cards) {
      delete card;
    }
    delete cards;

    cards = new std::vector<Card *>();
    for (auto card : *other.cards) {
      cards->push_back(new Card(*card));
    }
  }
  return *this;
}

Deck::~Deck() {
  for (auto card : *cards) {
    delete card;
  }
  delete cards;
}

Card &Deck::draw() {
  assert(cards != nullptr && !cards->empty());

  Card *drawnCard = std::move(cards->back());
  cards->pop_back();

  return *drawnCard;
}

void Deck::returnCard(Card *card) { cards->push_back(card); }

int Deck::size() const { return (int)cards->size(); }

std::ostream &operator<<(std::ostream &os, const Deck &deck) {
  os << "Deck[" << deck.cards->size() << " cards]: ";
  for (size_t i = 0; i < deck.cards->size(); i++) {
    if (i > 0)
      os << ", ";
    os << *(*deck.cards)[i];
  }
  return os;
}

// ------------- Hand Implementation ---------------

Hand::Hand(Deck *deck) {
  cards = new std::vector<Card *>();
  // Fill the hand with cards from the deck
  // for (int i = 0; i < handSize; i++) {
  //   cards->push_back(&deck->draw());
  // }
}

Hand::Hand(const Hand &other) {
  cards = new std::vector<Card *>();
  for (auto card : *other.cards) {
    cards->push_back(new Card(*card));
  }
}

Hand &Hand::operator=(const Hand &other) {
  if (this != &other) {
    for (auto card : *cards) {
      delete card;
    }
    delete cards;

    cards = new std::vector<Card *>();
    for (auto card : *other.cards) {
      cards->push_back(new Card(*card));
    }
  }
  return *this;
}

Hand::~Hand() {
  for (auto card : *cards) {
    delete card;
  }

  delete cards;
}

void Hand::drawFromDeckMultiple(int x, Deck *deck) {
    for (int i = 0; i < x; i++) {
        cards->push_back(&deck->draw());
    }
}

void Hand::playCard(int index, Deck &deck) {
  if (index < 0 || index >= (int)cards->size()) {
    std::cout << "Invalid card index." << std::endl;
    return;
  }

  Card *card = (*cards)[index];
  card->play();

  // Return card to deck
  deck.returnCard(card);

  // Remove from hand (don't delete — deck owns it now)
  cards->erase(cards->begin() + index);
}

int Hand::size() const { return (int)cards->size(); }

std::ostream &operator<<(std::ostream &os, const Hand &hand) {
  os << "Hand[" << hand.cards->size() << " cards]: ";
  for (size_t i = 0; i < hand.cards->size(); i++) {
    if (i > 0)
      os << ", ";
    os << "[" << i << "] " << *(*hand.cards)[i];
  }
  return os;
}