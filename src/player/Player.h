#pragma once

#include <string>
#include <vector>
#include <iostream>

class Territory;
class Deck;
class Hand;
class OrderList;

class Player {
private:
    int* id;
    static int* nextId;

    std::string* name;
    std::vector<Territory*>* territories;
    Deck* deck;
    Hand* hand;
    OrderList* orders;

public:
    Player(std::string name, Deck* deck);
    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    friend std::ostream& operator<<(std::ostream& os, const Player& p);

    std::string getName() const;
    const std::vector<Territory*>& getTerritories() const;
    Hand* getHand() const;
    OrderList* getOrders() const;

    void addTerritory(Territory* territory);
    void removeTerritory(Territory* territory);

    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;

    void issueOrder();

};