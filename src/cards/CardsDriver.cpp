#include <iostream>
#include "Cards.h"
int run_cards_driver() {

	std::cout << "Cards driver" << std::endl;

	int deckSize = 25;
	std::vector<Card*>* cards = new std::vector<Card*>();

	// Generating a bunch of cards for the deck
	for (int i = 0; i < deckSize; i++) {
		int rndType = rand() % 5;
		cards->push_back(new Card(static_cast<CardType>(rndType)));
	}

	Deck* deck = new Deck(*cards);

	Hand* hand = new Hand(deck);

	for (auto card : *hand->cards) {
		card->play();
	}

	return 0;
}