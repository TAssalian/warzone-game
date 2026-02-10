#pragma once
#include <vector>

enum CardType {
	Bomb,
	Reinforcement,
	Blockade,
	Airlift,
	Diplomacy
};

class Card {
public:
	CardType type;
	void play();

	Card(CardType type);
	~Card();
};

class Deck {
public:
	Card& draw();
	Deck(std::vector<Card*>& cards);
	~Deck();
private:
	std::vector<Card*>* cards;
};

class Hand {
public:
	Hand(Deck* deck);
	~Hand();
	std::vector<Card*>* cards;
private:
	static int const handSize = 6;
};