#include "Cards.h"

void Card::play()
{

}

Card::Card(CardType type) : type(type)
{
}

void Deck::draw()
{
	
}

Deck::Deck(std::vector<Card*>& cards) : cards(&cards)
{
};


