#include "Cards.h"
#include <iostream>
#include <assert.h>

void Card::play()
{

}

Card::Card(CardType type) : type(type)
{
}

Card::~Card()
{	
}

Card& Deck::draw()
{
	assert(cards != nullptr && !cards->empty());

	Card* drawnCard = std::move(cards->back());
	cards->pop_back();

	return *drawnCard;
}

Deck::Deck(std::vector<Card*>& cards) : cards(&cards)
{
}

Deck::~Deck()
{
	for (auto card : *cards) {
		delete card;
	}

	delete cards;
}

Hand::Hand(Deck* deck)
{
	cards = new std::vector<Card*>();
	// Fill the hand with cards from the deck
	for (int i = 0; i < handSize; i++) {
		cards->push_back(&deck->draw());
	}
}

Hand::~Hand()
{
	for (auto card : *cards) {
		delete card;
	}

	delete cards;
}
