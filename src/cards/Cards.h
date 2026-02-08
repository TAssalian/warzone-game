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

};

class Deck {
public:
	void draw();
	Deck(std::vector<Card*>& cards);
private:
	std::vector<Card*>* cards;
};

class Hand {
private:
	std::vector<Card> cards;
};