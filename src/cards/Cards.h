#pragma once
#include <iostream>
#include <vector>

enum CardType { Bomb, Reinforcement, Blockade, Airlift, Diplomacy };

std::string cardTypeName(CardType t);

class Card {
public:
  CardType type;

  Card(CardType type);
  Card(const Card &other);
  Card &operator=(const Card &other);
  ~Card();

  void play();

  friend std::ostream &operator<<(std::ostream &os, const Card &card);
};

class Deck {
public:
  Card &draw();
  void returnCard(Card *card);
  int size() const;

  Deck();
  Deck(std::vector<Card *> &cards);
  Deck(const Deck &other);
  Deck &operator=(const Deck &other);
  ~Deck();

  friend std::ostream &operator<<(std::ostream &os, const Deck &deck);

private:
  std::vector<Card *> *cards;
};

class Hand {
public:
  Hand(Deck *deck);
  Hand(const Hand &other);
  Hand &operator=(const Hand &other);
  ~Hand();

  std::vector<Card *> *cards;

  void drawFromDeckMultiple(int x, Deck* deck);
  void playCard(int index, Deck &deck);
  int size() const;

  friend std::ostream &operator<<(std::ostream &os, const Hand &hand);

private:
  static int const handSize = 6;
};